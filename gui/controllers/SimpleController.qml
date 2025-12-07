import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    StartButton {
        id: playButton
        running: false
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 300
        anchors.bottomMargin: 32
        visible: false
        opacity: visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }

        onClicked: {
            if (running) {
                stopButton.visible = true
            }
        }
    }

    // --- Simulation stop button --
    StopButton {
        id: stopButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 256
        anchors.bottomMargin: 12
        visible: false
        opacity: visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }

        onClicked: {
            playButton.running = false
            stopButton.visible = false
        }
    }
}
