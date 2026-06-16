
#include <QDebug>
#include <QMediaDevices>

#include "camerastream.h"


// TODO:: impliment these, set up audio streaming, pitch QExpected to core team.........
std::expected<bool, std::string> CameraStream::streamingStarted()
{
    return std::unexpected("Error: Streaming failed to start");
};

std::expected<bool, std::string> CameraStream::streamingStopped()
{
    return std::unexpected("Error: Streaming failed to stop");
};

std::expected<bool, std::string> CameraStream::checkConnection()
{
    return std::unexpected("Error: No Connection found");
};


CameraStream::CameraStream(QObject* parent) : QObject(parent) {

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


}
