import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Button {
    id: control

    property var controller

    property bool running: !!controller &&
                           controller.state === SimpleController.Running
    property bool stopped: !!controller &&
                           controller.state === SimpleController.Stopped
    property bool readyOrPaused: !!controller &&
                                 (controller.state === SimpleController.Ready ||
                                  controller.state === SimpleController.Paused)
    property color runningColor: "#3498db"
    property color pauseColor: "#2ecc71"
    property color restartColor: "#e67e22"

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

            property color colorMain: stopped ? restartColor
                                              : (running ? runningColor : pauseColor)

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
            visible: readyOrPaused

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
            visible: running

            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.fillStyle = "white";
                let barWidth = width * 0.35;
                ctx.fillRect(0, 0, barWidth, height);
                ctx.fillRect(width - barWidth, 0, barWidth, height);
            }
        }

        // RESTART
        Canvas {
            id: iconRestart
            anchors.centerIn: parent
            width: parent.width * 0.50
            height: parent.height * 0.50
            visible: stopped

            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = "white";
                ctx.lineWidth = width * 0.12;
                ctx.lineCap = "round";

                let cx = width * 0.50;
                let cy = height * 0.55;
                let r  = width * 0.35;

                ctx.beginPath();
                ctx.arc(cx, cy, r, Math.PI * 0.25, Math.PI * 1.35, true);
                ctx.stroke();

                ctx.beginPath();
                let arrowX = cx - r * Math.cos(Math.PI * 1.35);
                let arrowY = cy - r * Math.sin(Math.PI * 1.35);
                let size = width * 0.22;

                ctx.moveTo(arrowX, arrowY);
                ctx.lineTo(arrowX - size * 0.6, arrowY + size * 0.2);
                ctx.lineTo(arrowX - size * 0.1, arrowY + size * 0.6);
                ctx.stroke();
            }
        }
    }

    onClicked: {
        if (!controller)
            return;

        switch (controller.state) {
        case SimpleController.Ready:
        case SimpleController.Paused:
            controller.start();
            break;
        case SimpleController.Running:
            controller.pause();
            break;
        case SimpleController.Stopped:
            controller.restart();
            break;
        }
    }
}
