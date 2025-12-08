import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Button {
    id: control

    width: 46
    height: 46
    hoverEnabled: true
    text: ""

    background: Item {
        anchors.fill: parent

        Rectangle {
            id: bg
            anchors.fill: parent
            radius: width / 2

            property color colorMain: "#e74c3c"

            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.lighter(bg.colorMain, 1.25) }
                GradientStop { position: 0.55; color: bg.colorMain }
                GradientStop { position: 1.0; color: Qt.darker(bg.colorMain, 1.25) }
            }

            border.width: control.hovered ? 2 : 1
            border.color: "#ffffff40"

            scale: control.down ? 0.93 : 1.0
            Behavior on scale { NumberAnimation { duration: 80 } }
        }

        MultiEffect {
            source: bg
            anchors.fill: bg
            blurEnabled: true
            blur: 0.25
            colorizationColor: "white"
            colorization: 0.08
        }
    }

    contentItem: Item {
        anchors.fill: parent

        Rectangle {
            anchors.centerIn: parent
            width: control.width * 0.35
            height: control.height * 0.35
            radius: 0
            color: "white"
        }
    }
}
