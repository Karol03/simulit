import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property var stat: null

    implicitHeight: row.implicitHeight
    implicitWidth: row.implicitWidth

    HoverHandler {
        id: hover
    }

    property bool hovered: hover.hovered

    ToolTip.visible: hovered && stat?.hint !== ""
    ToolTip.text: stat?.hint ?? ""

    RowLayout {
        id: row
        anchors.fill: parent
        spacing: 4

        Label {
            id: labelItem
            font.family: "Source Sans 3"
            font.pixelSize: 12
            font.bold: true

            Layout.preferredWidth: 90
            Layout.minimumWidth: 90
            Layout.maximumWidth: 90

            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText

            text: stat?.label ?? ""
        }

        Label {
            id: valueItem
            font.family: "Source Sans 3"
            font.pixelSize: 11

            Layout.alignment: Qt.AlignRight
            text: typeof stat?.value === "number" && Math.floor(stat.value) !== stat.value
                  ? stat.value.toFixed(3)
                  : (stat?.value ?? "")
        }
    }
}
