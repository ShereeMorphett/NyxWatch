#ifndef CAMERASTREAM_H
#define CAMERASTREAM_H

#include <QObject>
#include <QCamera>
#include <QVideoSink>
#include <QMediaCaptureSession>

class CameraStream : public QObject
{
    Q_OBJECT

private:
    QCamera *m_camera;
    QVideoSink *m_videoSink;
    QMediaCaptureSession m_captureSession; // TODO:: NEED TO WORK THIS OUT FURTHER - https://doc.qt.io/qt-6/qmediacapturesession.html
    bool m_isStreaming;

public:
    bool const isStreaming() { return m_isStreaming; };
    explicit CameraStream(QObject *parent = nullptr);
};

#endif // CAMERASTREAM_H
