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
        spacing: 8

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
            text: root.value === null ? "" : String(root.value)

            onTextEdited: {
                root.value = text;
                root.valueChanged();
            }

            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }

    // int
    Component {
        id: intEditor
        SpinBox {
            from: -2147483648
            to: 2147483647
            value: root.value === null ? 0 : Number(root.value)

            onValueModified: {
                root.value = value;
                root.valueChanged();
            }

            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }

    // double (TextField + DoubleValidator)
    Component {
        id: doubleEditor
        TextField {
            inputMethodHints: Qt.ImhFormattedNumbersOnly

            text: root.value === null
                  ? ""
                  : Qt.locale().toString(Number(root.value), 'f', 3)

            validator: DoubleValidator {
                bottom: -1e9
                top: 1e9
                decimals: 6
            }

            onEditingFinished: {
                var v = Qt.locale().toDouble(text);
                if (isNaN(v))
                    v = 0.0;
                root.value = v;
                root.valueChanged();
            }

            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }

    // bool
    Component {
        id: boolEditor
        CheckBox {
            checked: root.value === null ? false : Boolean(root.value)

            onToggled: {
                root.value = checked;
                root.valueChanged();
            }

            ToolTip.visible: root.hovered && root.hint !== ""
            ToolTip.text: root.hint
        }
    }
}
