#include <QCoreApplication>
#include <QDebug>

#include "audiostream.h"
#include "camerastream.h"
#include "networkserver.h"

// using namespace std;

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    qDebug() << "program starting";
    qDebug() << "Calling camera stream";
    CameraStream stream;

    qDebug() << "Calling Audio stream";
    AudioStream audio;

    qDebug() << "Calling Network Server";
    NetworkServer server;

    QObject::connect(&stream, &CameraStream::frameCaptured,
                     &server, &NetworkServer::broadcastFrame);
    QObject::connect(&audio, &AudioStream::audioLevelsCalculated,
                     &server, &NetworkServer::broadcastAudioLevel);

    auto result = stream.streamingStarted();

    if (!result) {
        qCritical() << "[NyxWatch Server]: Failed to start camera";
        return -1;
    }

    // Start Audio
    if (!audio.startAudioCapture()) {
        qCritical() << "[NyxWatch Server]: Failed to initialize microphone telemetry.";
        // Optional return -1; depending on if you want audio to be a fatal prerequisite
    }

    if (!server.startServer(4545)) {
        qCritical() << "Failed to start network server.";
        return -1;
    }

    return a.exec();
}
