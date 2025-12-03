import QtQuick
import QtQuick.Controls


Button {
    id: control

    property color normalColor: "#1c1b1f"
    property color hoverColor: "#2a292d"

    font.family: "Source Sans 3"
    font.pixelSize: 16
    height: 48
    hoverEnabled: true
    background: Rectangle {
        anchors.fill: parent
        anchors.rightMargin: 4
        border.width: 0
        border.color: "#1c1b1f"
        radius: 0
        color: control.hovered ? hoverColor : normalColor
    }
}
