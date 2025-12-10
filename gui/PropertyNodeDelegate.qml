import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
Item {
    id: root

    property var prop: null
    readonly property int depth: prop?.groupDepth ?? 0
    readonly property real depthScale: Math.max(0.6, 1 - depth * 0.12)

    // skala całego itemu
    scale: depthScale

    // lekkie wcięcie, żeby wizualnie pokazać zagnieżdżenie
    x: depth * 14

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
            padding: 4 * root.depthScale
            background: Rectangle {
                border.width: 0
                radius: 0
                color: Qt.darker(panelColor, 1.1)
            }

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 4 * root.depthScale

                Label {
                    text: prop?.label ?? ""
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter

                    font.family: "Source Sans 3"
                    font.bold: true
                    font.pixelSize: 14 * root.depthScale
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 2 * root.depthScale
                    color: Qt.lighter(panelColor, 1.2)
                }
            }
        }
    }
}
