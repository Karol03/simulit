import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var prop: null  // Property variable (Variable<T> or VariablesGroup)

    implicitWidth: column.implicitWidth
    implicitHeight: column.implicitHeight

    ColumnLayout {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right

        Loader {
            id: rootLoader
            Layout.fillWidth: true
            sourceComponent: prop ? (prop.isGroup ? groupDelegate : leafDelegate) : null
        }
    }

    // --- Single property (leaf) ---
    Component {
        id: leafDelegate
        PropertyItem {
            id: leafItem
            Layout.fillWidth: true
            label: prop?.label ?? ""
            type: prop?.type ?? ""
            hint: prop?.hint ?? ""
            Component.onCompleted: value = prop?.value
            onValueChanged: prop && (prop.value = value)
        }
    }

    // --- Properties group ---
    Component {
        id: groupDelegate
        Frame {
            Layout.fillWidth: true
            padding: 4
            background: Rectangle {
                border.width: 0
                radius: 0
                color: panelColor
            }

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 6

                Item { height: 12 }

                Label {
                    text: prop?.label ?? ""
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter

                    font.family: "Source Sans 3"
                    font.bold: true
                    font.pixelSize: 14
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 2
                    color: Qt.lighter(panelColor, 1.2)
                }

                Item { height: 6 }
            }
        }
    }
}
