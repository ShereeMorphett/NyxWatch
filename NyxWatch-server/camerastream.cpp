
#include <QDebug>
#include <QMediaDevices>

#include "camerastream.h"

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
