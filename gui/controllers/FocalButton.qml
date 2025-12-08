import QtQuick
import QtQuick.Controls
import QtQuick.Effects


Button {
    id: control

    property var controller

    property bool readyOrPaused: controller.state === SimpleController.Ready ||
                                 controller.state === SimpleController.Paused
    property bool running: controller.state === SimpleController.Running
    property bool stopped: controller.state === SimpleController.Stopped
    property color readyOrPausedColor: "#2ecc71"
    property color runningColor: "#3498db"
    property color stoppedColor: "#e68522"

    property real baseScale: 1.0
    property real hoverScale: 1.05
    property real pressScale: 0.93
    property real currentScale: 0.0

    function updateScale() {
        currentScale = control.down ? pressScale
                                    : (control.hovered ? hoverScale : baseScale);
    }

    width: 66
    height: 66
    hoverEnabled: true
    text: ""

    transform: Scale {
        id: scaleTransform
        origin.x: control.width / 2
        origin.y: control.height / 2
        xScale: control.currentScale
        yScale: control.currentScale
    }

    Behavior on currentScale {
        NumberAnimation {
            duration: 160
            easing.type: Easing.OutBack
        }
    }

    Component.onCompleted: {
        updateScale()
    }

    onHoveredChanged: updateScale()
    onDownChanged: updateScale()

    background: Item {
        anchors.fill: parent

        Rectangle {
            id: bg
            anchors.fill: parent
            radius: width / 2

            property color colorMain: readyOrPaused ? readyOrPausedColor
                                                    : (running ? runningColor : stoppedColor)

            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.lighter(bg.colorMain, 1.25) }
                GradientStop { position: 0.55; color: bg.colorMain }
                GradientStop { position: 1.0; color: Qt.darker(bg.colorMain, 1.25) }
            }

            border.width: control.hovered ? 2 : 1
            border.color: "#ffffff40"
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
            width: parent.width * 0.7
            height: parent.height * 0.7
            visible: stopped

            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();

                ctx.strokeStyle = "white";
                ctx.fillStyle   = "white";
                ctx.lineWidth   = width * 0.09;
                ctx.lineCap     = "round";

                let cx = width  * 0.47;
                let cy = height * 0.50;
                let r  = width  * 0.34;

                let startAngle = Math.PI * 1.42;
                let endAngle   = Math.PI * 2.02;

                // Łuk
                ctx.beginPath();
                ctx.arc(cx, cy, r, startAngle, endAngle, true);
                ctx.stroke();

                ctx.fillStyle = "white";
                ctx.beginPath();
                ctx.moveTo(width * 0.61, height * 0.5);
                ctx.lineTo(width * 0.99, height * 0.5);
                ctx.lineTo(width * 0.8, height * 0.22);
                ctx.closePath();
                ctx.fill();
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
