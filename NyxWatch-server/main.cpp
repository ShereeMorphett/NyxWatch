#include <QCoreApplication>
#include <QDebug>

#include "camerastream.h"
#include "networkserver.h"

// using namespace std;

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    qDebug() << "program starting";
    qDebug() << "Calling camera stream";
    CameraStream stream;

    qDebug() << "Calling Network Server";
    NetworkServer server;

    QObject::connect(&stream, &CameraStream::frameCaptured,
                     &server, &NetworkServer::broadcastFrame);

    auto result = stream.streamingStarted();

    if (!result) {
        qCritical() << "[NyxWatch Server]: Failed to start server";
        return -1;
    }

    if (!server.startServer(4545)) {
        qCritical() << "Failed to start network server.";
        return -1;
    }

    return a.exec();
}
