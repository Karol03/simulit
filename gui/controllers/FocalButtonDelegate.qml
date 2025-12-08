import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Button {
    id: control

    property bool running: false
    property color runningColor: "#3498db"
    property color pauseColor: "#2ecc71"

    width: 66
    height: 66
    hoverEnabled: true
    text: ""

    background: Item {
        anchors.fill: parent

        Rectangle {
            id: bg
            anchors.fill: parent
            radius: width / 2

            property color colorMain: control.running ? runningColor : pauseColor

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

        // PLAY
        Canvas {
            id: iconPlay
            anchors.centerIn: parent
            width: parent.width * 0.45
            height: parent.height * 0.45
            visible: !control.running

            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.fillStyle = "white";
                ctx.beginPath();
                ctx.moveTo(6, 0);
                ctx.lineTo(width, height / 2);
                ctx.lineTo(6, height);
                ctx.closePath();
                ctx.fill();
            }
        }

        // PAUSE
        Canvas {
            id: iconPause
            anchors.centerIn: parent
            width: parent.width * 0.45
            height: parent.height * 0.45
            visible: control.running

            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.fillStyle = "white";
                let barWidth = width * 0.35;
                ctx.fillRect(0, 0, barWidth, height);
                ctx.fillRect(width - barWidth, 0, barWidth, height);
            }
        }
    }

    onClicked: control.running = !control.running
}
