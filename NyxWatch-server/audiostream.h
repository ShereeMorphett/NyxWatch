#pragma once

#include <expected>
#include <QAudioDevice>
#include <QAudioSource>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QObject>

// https://doc.qt.io/qt-6/qaudiodevice.html

class AudioStream : public QObject
{
    Q_OBJECT

private:
    QAudioSource *m_audioSource;
    QIODevice *m_audioSourceDevice; // The internal data stream pipe
    bool m_isStreaming;

signals:
    void audioLevelsCalculated(double rms, int16_t maxAmplitude);

private slots:
    void handleAudioDataReady(); // fires when bytes hit the mic buffer



public:
    bool const isStreaming() { return m_isStreaming; };
    explicit AudioStream(QObject *parent = nullptr);
    std::expected<bool, std::string> startAudioCapture();
    std::expected<bool, std::string> stopAudioCapture();

};
