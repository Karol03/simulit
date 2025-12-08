import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var controller

    FocalButton {
        id: focalButton
        controller: root.controller
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 300
        anchors.bottomMargin: 32
        visible: controller ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    AssistButton {
        id: assistButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 256
        anchors.bottomMargin: 12
        visible : root.controller &&
                  (root.controller.state === SimpleController.Running ||
                   root.controller.state === SimpleController.Paused)
        opacity: visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }

        onClicked: root.controller.stop()
    }
}
