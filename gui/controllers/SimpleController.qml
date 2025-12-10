import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var controller
    property bool propertiesExpanded

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
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: root.propertiesExpanded ? 320 : 128
        anchors.bottomMargin: 92
        visible: controller !== null

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }

        Behavior on anchors.rightMargin {
            NumberAnimation {
                duration: 240
                easing.type: Easing.InOutQuad
            }
        }

        Binding {
            target: focalButton
            property: "state"
            value: root.controller ? root.controller.state : undefined
            when: root.controller !== null
        }
    }

    AssistButton {
        id: assistButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: root.propertiesExpanded ? 276 : 84
        anchors.bottomMargin: 72
        visible : root.controller &&
                  (root.controller.state === ControllerState.Running ||
                   root.controller.state === ControllerState.Paused)
        opacity: visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }

        Behavior on anchors.rightMargin {
            NumberAnimation {
                duration: 240
                easing.type: Easing.InOutQuad
            }
        }

        onClicked: root.controller.stop()
    }
}
