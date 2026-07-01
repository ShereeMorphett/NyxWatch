import QtQuick
import QtQuick.Controls
import QtMultimedia
import NyxWatch_client

Window {
    id: root
    width: 800
    height: 480
    visible: true
    title: "NyxWatch Client Proof of Concept"
    color: "#1e1e1e"

    ClientStream {
        id: networkBackend
    }

    Connections {
        target: networkBackend
        function onSoundAlertTriggered(rms) {
            alertText.text = "Sound Alert Triggered! (RMS: " + rms.toFixed(2) + ")"
            alertBanner.visible = true
            alertTimer.restart()
        }
    }

    VideoOutput {
        id: outputDisplay
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectFit
    }

    Rectangle {
        id: alertBanner
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        width: alertText.implicitWidth + 40
        height: 50
        color: "#d9534f" //  red
        radius: 8
        visible: false
        opacity: visible ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation { duration: 250 }
        }

        Text {
            id: alertText
            anchors.centerIn: parent
            color: "white"
            font.bold: true
            font.pointSize: 14
        }
    }

    Timer {
        id: alertTimer
        interval: 3000
        repeat: false
        onTriggered: alertBanner.visible = false
    }

    Component.onCompleted: {
        console.log("Client UI ready, configuring hooks...")
        networkBackend.videoSink = outputDisplay.videoSink
        networkBackend.connectToServer("127.0.0.1", 4545)
    }
}