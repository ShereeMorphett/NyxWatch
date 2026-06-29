#include "clientstream.h"
#include <QDataStream>
#include <QVideoFrameFormat>

ClientStream::ClientStream(QObject *parent): QObject(parent), m_tcpSocket(new QTcpSocket(this))
{
    // Whenever the socket receives network bytes, jump into our parser
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ClientStream::parseIncomingPackets);

    connect(m_tcpSocket, &QTcpSocket::connected, []() { qInfo() << "[NyxWatch Client][ClientStream]Connected to NyxWatch Server!"; });
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

    while (m_tcpSocket->bytesAvailable() >= 16) {
        quint32 bytesCount, width, height, pixelFormat;

        stream.startTransaction();

        stream >> bytesCount;
        stream >> width;
        stream >> height;
        stream >> pixelFormat;

        // Check if the actual video payload bytes have completely arrived
        if (m_tcpSocket->bytesAvailable() < bytesCount) {
            stream.rollbackTransaction(); // Not enough data yet, wait for next readyRead
            return;
        }

        // Read the raw frame bytes out of the socket buffer
        QByteArray pixelData = m_tcpSocket->read(bytesCount);

        if (!stream.commitTransaction())
            return; // Stream check failed

        // Reconstruct the QVideoFrame using the metadata from server
        QVideoFrameFormat frameFormat(QSize(width, height), static_cast<QVideoFrameFormat::PixelFormat>(pixelFormat));
        QVideoFrame frame(frameFormat);

        // Map it so we can copy our network bytes into client memory
        if (frame.map(QVideoFrame::WriteOnly)) {
            memcpy(frame.bits(0), pixelData.constData(), bytesCount);
            frame.unmap();

            // Push it straight up to the QML VideoOutput element
            if (m_videoSink)
                m_videoSink->setVideoFrame(frame);
        }
    }
}

    //Validate and move to the .cpp
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
            m_videoSink->setVideoFrame(frame); // This renders it in QML
    }
