import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var prop: null  // IProperty (Property or PropertyGroup)

    implicitWidth: column.implicitWidth
    implicitHeight: column.implicitHeight

    ColumnLayout {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right

        Loader {
            id: rootLoader

            property var prop: root.prop

            active: prop !== null
            Layout.fillWidth: true
            sourceComponent: prop && prop.isGroup ? groupDelegate : leafDelegate
        }
    }

    // --- Single property (leaf) ---
    Component {
        id: leafDelegate
        PropertyItem {
            id: leafItem
            Layout.fillWidth: true
            label: prop.label
            type: prop.type
            hint: prop.hint
            Component.onCompleted: value = prop.value
            onValueChanged: prop.value = value
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
                spacing: 4

                Label {
                    text: prop.label
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

                Repeater {
                    model: prop.children

                    delegate: Loader {
                        Layout.fillWidth: true
                        property var prop: modelData
                        sourceComponent: prop && prop.isGroup ? groupDelegate : leafDelegate
                    }
                }

                Item { height: 18 }
            }
        }
    }
}
