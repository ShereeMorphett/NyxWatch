#pragma once

#include <QObject>
#include <QCamera>
#include <expected>
#include <QMediaCaptureSession>


// https://doc.qt.io/qt-6/qaudiodevice.html

class AudioStream : public QObject
{
    Q_OBJECT

private:
    QCamera *m_camera;

    QMediaCaptureSession m_captureSession; // TODO:: NEED TO WORK THIS OUT FURTHER - https://doc.qt.io/qt-6/qmediacapturesession.html
    bool m_isStreaming;

signals:
    void frameCaptured(const QVideoFrame &frame);

private slots:
    void handleNewFrame(const QVideoFrame &frame);
    void handleCameraError(QCamera::Error error, const QString &errorString);

public:
    bool const isStreaming() { return m_isStreaming; };
    explicit AudioStream(QObject *parent = nullptr);

    // TODO:: impliment these, set up audio streaming, pitch QExpected to core team.........
    std::expected<bool, std::string> audioStreamingStarted();
    std::expected<bool, std::string> audioStreamingStopped();
    std::expected<bool, std::string> checkConnection();
};
