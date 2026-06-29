import QtQuick
import QtQuick.Controls
import QtMultimedia
import NyxWatch_client

Window {
    width: 800
    height: 480
    visible: true
    title: "NyxWatch Client Proof of Concept"
    color: "#1e1e1e"

    ClientStream {
        id: networkBackend
    }

    VideoOutput {
        id: outputDisplay
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectFit

    }

    Component.onCompleted: {
        console.log("Client UI ready, configuring hooks...")
        networkBackend.videoSink = outputDisplay.videoSink
        networkBackend.connectToServer("127.0.0.1", 4545)
    }
}