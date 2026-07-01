#include <QDebug>
#include <QMediaDevices>

#include "camerastream.h"

void CameraStream::handleCameraError(QCamera::Error error, const QString &errorString)
{
    m_isStreaming = false;

    qCritical() << "[NyxWatch Server][CameraStream] Native Camera Error Encounted!";
    qCritical() << "   Error Code:" << error;
    qCritical() << "   Details:" << errorString;
}

// TODO:: impliment these, set up audio streaming
std::expected<bool, std::string> CameraStream::streamingStarted()
{
    if (!m_camera)
        return std::unexpected("Error: No camera detected");
    if (m_isStreaming)
        return std::unexpected("Error: Stream is already running");

    qInfo() << "[NyxWatch Server]: Camera stream starting";
    m_camera->start();
    m_isStreaming = true;
    return true;
};

std::expected<bool, std::string> CameraStream::streamingStopped()
{
    if (!m_isStreaming)
        return std::unexpected("Error: Stream is not currently running");
    if (!m_camera)
        return std::unexpected("Error: No camera detected");

    qInfo() << "[NyxWatch Server]: Camera stream stopping";
    m_camera->stop();
    m_isStreaming = false;
    return true;
};

void CameraStream::handleNewFrame(const QVideoFrame &frame)
{
    if (!frame.isValid())
        return;
    emit frameCaptured(frame);
}

std::expected<bool, std::string> CameraStream::checkConnection()
{
    return std::unexpected("Error: No Connection found");
};

CameraStream::CameraStream(QObject* parent) : QObject(parent), m_camera(nullptr),
                                            m_videoSink(nullptr), m_isStreaming(false) {

    if (QMediaDevices::videoInputs().empty()) {
        qCritical() << "No camera detected on the Raspberry Pi!";
        return;
    } else {
        // Logging for learning purposes, remove later
        qCritical() << "Found camera:   " << QMediaDevices::videoInputs().size();
        for (const QCameraDevice &cameraDevice : QMediaDevices::videoInputs()) {
            qInfo() << "-----------------------------------------";
            qInfo() << "Camera Name/ID:  " << cameraDevice.id();
            qInfo() << "Description:     " << cameraDevice.description();
            qInfo() << "Is Default View: " << cameraDevice.isDefault();
        }
        qInfo() << "-----------------------------------------";
    }

    m_camera = new QCamera(QMediaDevices::defaultVideoInput(), this);
    m_videoSink = new QVideoSink(this);

    m_captureSession.setCamera(m_camera);
    m_captureSession.setVideoSink(m_videoSink);

    connect(m_videoSink, &QVideoSink::videoFrameChanged,
            this, &CameraStream::handleNewFrame);
    connect(m_camera, &QCamera::errorOccurred,
            this, &CameraStream::handleCameraError);

}
