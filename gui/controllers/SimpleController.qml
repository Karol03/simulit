import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var controller

    Dialog {
        id: errorDialog
        title: "Błąd"
        modal: true
        standardButtons: Dialog.Ok
        anchors.centerIn: parent

        Text {
            id: errorText
            text: ""
            wrapMode: Text.WordWrap
            padding: 12
        }
    }

    Connections {
        target: root.controller

        function onError(message) {
            errorText.text = message
            errorDialog.open()
        }
    }

    FocalButton {
        id: focalButton
        state: root.controller.state
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 330
        anchors.bottomMargin: 62
        visible: controller ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    AssistButton {
        id: assistButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 286
        anchors.bottomMargin: 42
        visible : root.controller &&
                  (root.controller.state === ControllerState.Running ||
                   root.controller.state === ControllerState.Paused)
        opacity: visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }

        onClicked: root.controller.stop()
    }
}
