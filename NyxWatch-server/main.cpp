#include <QCoreApplication>
#include <QDebug>

#include "camerastream.h"

using namespace std;

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    qDebug() << "program starting";
    qDebug() << "Calling camera stream";
    CameraStream stream;
    // Start the stream using your clean std::expected pattern
    auto result = stream.streamingStarted();

    if (!result) {
        qCritical() << "FAILED TO START SERVER:" << QString::fromStdString(result.error());
        return -1;
    }

    return a.exec();
}
