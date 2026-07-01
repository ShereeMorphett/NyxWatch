#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QVideoSink>
#include <QVideoFrame>
#include <QtQml/qqmlregistration.h> // This needs to be handled a better way

class ClientStream : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)

private:
    QVideoSink* m_videoSink = nullptr;
    QTcpSocket* m_tcpSocket = nullptr;

    void parseIncomingPackets();

signals:
    void videoSinkChanged();
    void soundAlertTriggered(double level);

public:
    explicit ClientStream(QObject *parent = nullptr);

   Q_INVOKABLE void connectToServer(const QString &host, quint16 port);

   QVideoSink* videoSink() const;
   void setVideoSink(QVideoSink* sink);
   void processIncomingFrame(const QVideoFrame &frame);
};
