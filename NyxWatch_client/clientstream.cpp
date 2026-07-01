#include "clientstream.h"
#include <QDataStream>
#include <QVideoFrameFormat>

ClientStream::ClientStream(QObject *parent): QObject(parent), m_tcpSocket(new QTcpSocket(this))
{
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ClientStream::parseIncomingPackets);
    connect(m_tcpSocket, &QTcpSocket::connected, []()
            { qInfo() << "[NyxWatch Client][ClientStream]Connected to NyxWatch Server!"; });
}

void ClientStream::connectToServer(const QString &host, quint16 port)
{
    qInfo() << "[NyxWatch Server][ClientStream] Connecting to server at" << host << ":" << port;
    m_tcpSocket->connectToHost(host, port);
}

void ClientStream::parseIncomingPackets()
{
    QDataStream stream(m_tcpSocket);
    stream.setVersion(QDataStream::Qt_6_5);

    while (true) {
        stream.startTransaction();
        quint32 packetType = 0;
        stream >> packetType;

        if (stream.status() == QDataStream::ReadPastEnd) {
            stream.rollbackTransaction();
            break; // Stop looping, wait for more network data
        }

        // --- CASE 1: IT'S A VIDEO FRAME ---
        if (packetType == 1) {
            quint32 bytesCount = 0, width = 0, height = 0, pixelFormat = 0;
            stream >> bytesCount >> width >> height >> pixelFormat;

            QByteArray pixelData;
            pixelData.resize(bytesCount);
            int readBytes = stream.readRawData(pixelData.data(), bytesCount);

            if (stream.status() == QDataStream::ReadPastEnd || readBytes < static_cast<int>(bytesCount)) {
                stream.rollbackTransaction(); // Rewind everything (forgetting the packetType is silly, dont do that)
                break;
            }

            if (!stream.commitTransaction())
                break;

            // Reconstruct and push frame to QML
            QVideoFrameFormat frameFormat(QSize(width, height), static_cast<QVideoFrameFormat::PixelFormat>(pixelFormat));
            QVideoFrame frame(frameFormat);

            if (frame.map(QVideoFrame::WriteOnly)) {
                memcpy(frame.bits(0), pixelData.constData(), bytesCount);
                frame.unmap();
                if (m_videoSink) m_videoSink->setVideoFrame(frame);
            }
        }

        // --- CASE 2: IT'S AUDIO TELEMETRY ---
        else if (packetType == 2) {
            float rms = 0.0f;
            quint16 maxAmplitude = 0;
            stream >> rms >> maxAmplitude;

            if (stream.status() == QDataStream::ReadPastEnd) {
                stream.rollbackTransaction();
                break;
            }

            if (!stream.commitTransaction())
                break;

            bool typingOrTap = (maxAmplitude > 600);
            bool talkOrMeow  = (rms > 130.0 && maxAmplitude > 350);

            if (typingOrTap || talkOrMeow) {
                qWarning() << "[NyxWatch Client] - Valid Event Triggered RMS:" << rms << "Peak:" << maxAmplitude;
                emit soundAlertTriggered(rms);
            }
        } else {
            qCritical() << "[NyxWatch Client] Protocol sync lost! Unknown type:" << packetType;
            stream.rollbackTransaction();
            m_tcpSocket->disconnectFromHost();
            return;
        }
    }
}

QVideoSink* ClientStream::videoSink() const {
    return m_videoSink;
}

void ClientStream::setVideoSink(QVideoSink* sink) {
    if (m_videoSink != sink) {
        m_videoSink = sink;
        emit videoSinkChanged();
    }
}

void ClientStream::processIncomingFrame(const QVideoFrame &frame) {
    if (m_videoSink)
        m_videoSink->setVideoFrame(frame);
}
