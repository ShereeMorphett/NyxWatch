#pragma once

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>

class ClientStream : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)

private:
    QVideoSink* m_videoSink = nullptr;

signals:
    void videoSinkChanged();

public:
    explicit ClientStream(QObject *parent = nullptr) : QObject(parent) {}

    QVideoSink* videoSink() const { return m_videoSink; }
    void setVideoSink(QVideoSink* sink) {
        if (m_videoSink != sink) {
            m_videoSink = sink;
            emit videoSinkChanged();
        }
    }

    // You will call this function whenever a full frame arrives over your network socket
    void processIncomingFrame(const QVideoFrame &frame) {
        if (m_videoSink) {
            m_videoSink->setVideoFrame(frame); // This instantly renders it in QML
        }
    }
};
