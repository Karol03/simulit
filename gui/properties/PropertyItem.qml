import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    property string label: ""
    property string type: "text"     // "text" | "int" | "double" | "bool"
    property var value: null
    property string hint: ""

    implicitHeight: row.implicitHeight
    implicitWidth: row.implicitWidth

    HoverHandler {
        id: hover
    }

    property bool hovered: hover.hovered

    // --- tooltip ---
    ToolTip.visible: hovered && hint !== ""
    ToolTip.text: hint

    ColumnLayout {
        id: row
        anchors.fill: parent
        spacing: 4

        Label {
            id: hintItem
            font.family: "Source Sans 3"
            font.pixelSize: 12
            Layout.alignment: Qt.AlignHCenter
            text: root.label
        }

        Loader {
            id: editorLoader
            Layout.fillWidth: true
            sourceComponent: {
                switch (root.type) {
                case "int":    return intEditor;
                case "double": return doubleEditor;
                case "bool":   return boolEditor;
                default:       return textEditor;
                }
            }
        }
    }

    // --- different editors, dependent on type ---

    // text
    Component {
        id: textEditor
        TextField {
            text: prop.value === null ? "" : String(prop.value)
            onEditingFinished: {
                prop.value = text;
            }
            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }

    // int
    Component {
        id: intEditor
        TextField {
            text: prop.value === null ? "" : String(prop.value)
            onEditingFinished: {
                prop.value = text;
            }
            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }

    // double (TextField + DoubleValidator)
    Component {
        id: doubleEditor
        TextField {
            text: prop.value === null ? "" : String(prop.value)
            onEditingFinished: {
                prop.value = text;
            }
            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }

    // bool
    Component {
        id: boolEditor
        CheckBox {
            checked: prop.value === null ? false : Boolean(prop.value)
            onToggled: {
                prop.value = checked;
            }
            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }
}
