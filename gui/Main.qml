import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Shapes
import QtQuick.Dialogs
import QtQuick.Controls.Material

import "properties"


ApplicationWindow {
    id: root
    width: 1080
    height: 720
    visible: true
    title: qsTr("Simulit")
    Material.theme: Material.Dark
    Material.accent: Material.DeepPurple
    Material.foreground: Material.Grey

    // --- Panel state ---
    property bool propertiesOpen: false
    property bool panelOpen: false
    property color normalColor: "#1c1b1f"
    property color hoverColor: "#2a292d"
    property color panelColor: "#333"

    FontLoader {
        id: materialIcons
        source: "qrc:/fonts/resources/MaterialSymbolsSharp-Regular.ttf"
    }

    // --- Error dialog ---
    MessageDialog {
        id: errorDialog
        title: "Błąd"
        text: ""
        visible: false
    }

    function showError(msg) {
        errorDialog.text = msg
        errorDialog.open()
    }

    Connections {
        target: simulationProvider
        function onErrorOccurred(message) {
            showError(message)
        }
    }

    // --- Main frame for single simulation ---
    Frame {
        id: simulationViewFrame
        anchors.fill: parent
        anchors.margins: 0
        padding: 0
        background: Rectangle {
            border.width: 0
            radius: 0
            color: normalColor
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (sidePanel.x === 0)
                    panelOpen = !panelOpen
            }
        }

        // --- Simulation properties panel on the right--
        Rectangle {
            id: propertiesPanel
            width: 240
            height: parent.height
            color: panelColor
            clip: true

            x: propertiesOpen ? parent.width - width : parent.width

            Behavior on x {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            ScrollView {
                id: propertiesScroll
                anchors.fill: parent
                anchors.margins: 12

                ColumnLayout {
                    id: propertiesColumn
                    width: propertiesScroll.availableWidth
                             - propertiesScroll.ScrollBar.vertical.width
                    spacing: 4

                    Repeater {
                        model: simulationProvider ? simulationProvider.properties : []
                        delegate: PropertyNodeDelegate {
                            Layout.fillWidth: true
                            prop: modelData
                        }
                    }

                    // opcjonalny "dół" na oddech
                    Item { Layout.fillWidth: true; height: 18 }
                }
            }
        }
    }

    // --- Button for menu panel ---
    Button {
        id: openButton
        text: "\uE5D2"        // 'menu'
        font.family: materialIcons.name
        font.pixelSize: 24
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        onClicked: {
            if (sidePanel.x !== 0)
                panelOpen = !panelOpen
        }
    }

    // --- Menu panel on the left ---
    Rectangle {
        id: sidePanel
        width: 220
        height: parent.height
        color: panelColor
        anchors.top: parent.top

        x: panelOpen ? 0 : -width

        Behavior on x {
            NumberAnimation {
                duration: 150
                easing.type: Easing.InOutQuad
            }
        }

        Column {
            id: sidePanelColumn
            anchors.fill: parent
            anchors.margins: 0
            anchors.topMargin: 2
            spacing: 1

            Repeater {
                model: simulationProvider ? simulationProvider.simulations : []
                delegate: MenuButton {
                    width: parent.width
                    text: modelData
                    onClicked: {
                        root.panelOpen = false
                        simulationProvider.select(text)
                        root.propertiesOpen = true
                    }
                }
            }
        }
    }
}
