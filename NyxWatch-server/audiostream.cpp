#include <QDebug>
#include <QMediaDevices>
#include "audiostream.h"


AudioStream::AudioStream(QObject *parent) : QObject(parent), m_audioSource(nullptr),
                                            m_audioSourceDevice(nullptr), m_isStreaming(false)
{}

std::expected<bool, std::string> AudioStream::startAudioCapture()
{
    const auto devices = QMediaDevices::audioInputs();
    QAudioDevice targetDevice;

    // Force the server to explicitly lock onto your back 3.5mm jack
    QString targetMicName = "Built-in Audio Analog Stereo";

    for (const QAudioDevice &device : devices) {
        if (device.description() == targetMicName) {
            targetDevice = device;
            break;
        }
    }

    if (targetDevice.isNull()) {
        qCritical() << "[NyxWatch Server][AudioStream] CRITICAL: Built-in analog audio hardware missing!";
        return std::unexpected("Built-in Audio device context not found.");
    }

    qInfo() << "[NyxWatch Server][AudioStream] Lock secured on Jack Hardware:" << targetDevice.description();

    // Define our standard contract
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!targetDevice.isFormatSupported(format)) {
        format = targetDevice.preferredFormat();
    }

    m_audioSource = new QAudioSource(targetDevice, format, this);
    m_audioSourceDevice = m_audioSource->start();

    if (!m_audioSourceDevice) {
        qCritical() << "[NyxWatch Server][AudioStream] Failed to open audio source device context.";
        return std::unexpected("Failed to open audio hardware stream context.");
    }

    connect(m_audioSourceDevice, &QIODevice::readyRead, this, &AudioStream::handleAudioDataReady);

    m_isStreaming = true;
    qInfo() << "[NyxWatch Server][AudioStream] Listening for incoming audio levels...";
    return true;
}

void AudioStream::handleAudioDataReady()
{
    if (!m_audioSourceDevice)
        return;

    QByteArray audioBytes = m_audioSourceDevice->readAll();
    if (audioBytes.isEmpty())
        return;

    const int16_t *samples = reinterpret_cast<const int16_t*>(audioBytes.constData());
    int sampleCount = audioBytes.size() / sizeof(int16_t);

    int16_t maxAmplitude = 0;
    double sumOfSquares = 0;

    for (int i = 0; i < sampleCount; ++i) {
        int16_t sampleValue = samples[i];

        if (std::abs(sampleValue) > maxAmplitude)
            maxAmplitude = std::abs(sampleValue);
        sumOfSquares += sampleValue * sampleValue;
    }

    // Calculate RMS (Root Mean Square) volume level- Root Mean Square- measures the effective power or continuous energy of an audio signal over a short window of time.
    double rms = 0;
    if (sampleCount > 0)
        rms = std::sqrt(sumOfSquares / sampleCount);

    // Convert raw amplitude numbers into a basic relative visual indicator meter
    int visualMeterWidth = static_cast<int>((rms / 32767.0) * 50.0);
    QString meterBar = QString("|").repeated(qBound(0, visualMeterWidth, 50));

    if (maxAmplitude > 250 || rms > 80.0)
        qDebug() << "[NyxWatch Server][AudioStream] ACTIVITY DETECTED RMS:" << static_cast<int>(rms) << "Peak:" << maxAmplitude;

    emit audioLevelsCalculated(rms, maxAmplitude);
}

std::expected<bool, std::string>  AudioStream::stopAudioCapture()
{
    if (!m_isStreaming)
        return std::unexpected("Error: Stream is not currently running");
    if (!m_audioSource)
        return std::unexpected("Error: No Mic detected");

    m_audioSource->stop();
    m_isStreaming = false;
    return true;
}
