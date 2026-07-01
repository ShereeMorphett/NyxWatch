#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVideoFrame>

class NetworkServer : public QObject
{
    Q_OBJECT

private:
    QTcpServer *m_tcpServer;
    QList<QTcpSocket*> m_clientSockets; // Track connected apps/clients

private slots:
    void handleNewConnection();
    void handleClientDisconnect();

public slots:
    // receives frames from the CameraStream
    void broadcastFrame(const QVideoFrame &frame);
    void broadcastAudioLevel(double rms, int16_t maxAmplitude);


public:
    explicit NetworkServer(QObject *parent = nullptr);
    bool startServer(quint16 port);

};