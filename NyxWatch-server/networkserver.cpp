#include "networkserver.h"

#include <QDataStream>

void NetworkServer::broadcastFrame(const QVideoFrame &frame)
{
    if (m_clientSockets.isEmpty())
        return;

    QVideoFrame cloneFrame(frame);

    if (cloneFrame.map(QVideoFrame::ReadOnly)) {

        int bytesCount = cloneFrame.mappedBytes(0);
        QVideoFrameFormat format = cloneFrame.surfaceFormat();

        int width = format.frameSize().width();
        int height = format.frameSize().height();
        int pixelFormat = static_cast<int>(format.pixelFormat());

        // Prepare our network packet buffer
        QByteArray packetBuffer;
        QDataStream stream(&packetBuffer, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_6_5); // Keep streams version predictable

        stream << (quint32)bytesCount;
        stream << (quint32)width;
        stream << (quint32)height;
        stream << (quint32)pixelFormat;

        packetBuffer.append(reinterpret_cast<const char*>(cloneFrame.bits(0)), bytesCount);

        for (QTcpSocket *client : m_clientSockets) {
            if (client->state() == QAbstractSocket::ConnectedState)
                client->write(packetBuffer);
        }

        // unmap needed to free the hardware resources
        cloneFrame.unmap();
    } else {
        qWarning() << "[NetworkServer] Failed to map video frame to CPU memory.";
    }

}

void NetworkServer::handleNewConnection()
{
    // Probably overkill but why not manage multiple clients coz I want to see if I can. Strip it out in debugging if i need to
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();

    if (!clientSocket) return;

    qInfo() << "[NyxWatch-Server][NetworkServer]  New client connected from IP:"
            << clientSocket->peerAddress().toString();

    m_clientSockets.append(clientSocket);

    connect(clientSocket, &QTcpSocket::disconnected,
            this, &NetworkServer::handleClientDisconnect);
}

void NetworkServer::handleClientDisconnect()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    qInfo() << "[NyxWatch-Server][NetworkServer] Client disconnected.";

    // Remove them from broadcast stream list
    m_clientSockets.removeAll(clientSocket);
    clientSocket->deleteLater();
}

bool NetworkServer::startServer(quint16 port)
{
    m_tcpServer = new QTcpServer(this);

    // QHostAddress: will accept connections over Localhost, Wi-Fi, or Ethernet
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        qCritical() << "[NyxWatch-Server][NetworkServer] Server failed to bind to port" << port
                    << "Reason:" << m_tcpServer->errorString();
        return false;
    }

    //handshake listener
    connect(m_tcpServer, &QTcpServer::newConnection, this, &NetworkServer::handleNewConnection);

    qInfo() << "[NyxWatch-Server][NetworkServer] TCP Socket Server actively listening on port" << port;
    return true;
}

NetworkServer::NetworkServer(QObject *parent): QObject(parent), m_tcpServer(nullptr) {}
