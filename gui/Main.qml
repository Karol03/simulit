import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Shapes
import QtQuick.Dialogs
import QtQuick.Controls.Material

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
    property bool simulationSelected: false
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
        target: simulationHandler
        function onErrorOccurred(message) {
            showError(message)
        }
    }

    // --- Main frame for single simulation ---
    Frame {
        id: simulationViewFrame
        anchors.fill: parent
        padding: 0
        background: Rectangle {
            border.width: 0
            radius: 0
            color: "transparent"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (sidePanel.x === 0)
                    panelOpen = !panelOpen
            }
        }

        // --- Simulation info header ---
        Rectangle {
            id: simulationInfoCard
            width: Math.min(parent.width * 0.46, 480)
            height: infoColumn.implicitHeight + 20
            radius: 8
            color: Qt.darker(panelColor, 1.05)
            border.width: 1
            border.color: "#444348"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            visible: simulationSelected && simulationHandler
            opacity: visible ? 1 : 0
            y: visible ? anchors.topMargin : anchors.topMargin + 20

            property bool descriptionExpanded: false

            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
            }

            Behavior on y {
                NumberAnimation { duration: 300; easing.type: Easing.OutQuad }
            }

            ToolButton {
                id: toggleDescButton
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 6
                anchors.rightMargin: 6

                text: simulationInfoCard.descriptionExpanded ? "\u25B2" : "\u25BC" // ▲ / ▼
                font.pixelSize: 10
                padding: 4
                background: Rectangle {
                    radius: 6
                    color: Qt.rgba(0, 0, 0, 0.2)
                }

                onClicked: simulationInfoCard.descriptionExpanded = !simulationInfoCard.descriptionExpanded
                ToolTip.visible: hovered
                ToolTip.text: simulationInfoCard.descriptionExpanded ?
                                  qsTr("Zwiń") : qsTr("Rozwiń opis")
            }

            ColumnLayout {
                id: infoColumn
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                Label {
                    text: simulationHandler && simulationHandler.name !== "" ?
                              simulationHandler.name : qsTr("Brak wybranej symulacji")
                    font.family: "Source Sans 3"
                    font.pixelSize: 18
                    font.bold: true
                    Layout.fillWidth: true
                    wrapMode: Text.NoWrap
                    elide: Text.ElideRight
                }

                Label {
                    text: simulationHandler && simulationHandler.description !== "" ?
                              simulationHandler.description : qsTr("Wybierz symulację z listy po lewej, aby zobaczyć jej opis.")
                    font.family: "Source Sans 3"
                    font.pixelSize: 13
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap

                    maximumLineCount: simulationInfoCard.descriptionExpanded ? 18 : 1
                    elide: Text.ElideRight
                }
            }
        }

        // --- Simulation properties panel on the right--
        Rectangle {
            id: propertiesPanel
            width: 260
            height: parent.height
            color: Qt.darker(panelColor, 1.1)
            clip: true
            topLeftRadius: 8
            bottomLeftRadius: 8
            border.width: 1
            border.color: "#444348"

            property bool panelExpanded: true
            property int collapsedVisibleWidth: 26

            x: simulationSelected
               ? parent.width - (panelExpanded ? width : collapsedVisibleWidth) - 8
               : parent.width + 8

            Behavior on x {
                NumberAnimation {
                    duration: 180
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
                             - 8
                    spacing: 8

                    Repeater {
                        model: simulationHandler ? simulationHandler.properties : []
                        delegate: PropertyNodeDelegate {
                            Layout.fillWidth: true
                            prop: modelData
                        }
                    }

                    Item { Layout.fillWidth: true; height: 6 }

                    Repeater {
                        model: simulationHandler ? simulationHandler.workerProperties : []
                        delegate: PropertyNodeDelegate {
                            Layout.fillWidth: true
                            prop: modelData
                        }
                    }

                    Item { Layout.fillWidth: true; height: 8 }
                }
            }

            ToolButton {
                id: togglePropertiesButton
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 8
                anchors.leftMargin: 8

                text: propertiesPanel.panelExpanded ? "\u25B6" : "\u25C0"   // ▶ / ◀
                font.pixelSize: 14
                padding: 4

                background: Rectangle {
                    radius: 6
                    color: Qt.rgba(0, 0, 0, 0.2)
                }

                onClicked: propertiesPanel.panelExpanded = !propertiesPanel.panelExpanded

                ToolTip.visible: hovered
                ToolTip.text: propertiesPanel.panelExpanded
                                ? qsTr("Zwiń panel")
                                : qsTr("Rozwiń panel właściwości")
            }
        }

        // --- Simulation statistics ---
        Rectangle {
            id: statisticsPanel
            width: parent.width * 0.5
            color: Qt.darker(panelColor, 1.1)
            clip: true
            radius: 8
            border.width: 1
            border.color: "#444348"
            x: parent.width * 0.1
            y: simulationSelected ? parent.height - height - 12 : parent.height + 14

            property bool statisticsExpanded: true
            property int expandedHeight: 212
            property int collapsedHeight: 56

            height: statisticsExpanded ? expandedHeight : collapsedHeight

            Behavior on height {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }

            RowLayout {
                id: headerRow
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 8
                spacing: 8

                Label {
                    text: qsTr("Statystyki")
                    font.family: "Source Sans 3"
                    font.pixelSize: 15
                    font.bold: true
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                    elide: Text.ElideRight
                }

                ToolButton {
                    id: toggleStatButton
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    text: statisticsPanel.statisticsExpanded ? "\u25BC" : "\u25B2" // ▼ / ▲
                    font.pixelSize: 10
                    padding: 4
                    background: Rectangle {
                        radius: 6
                        color: Qt.rgba(0, 0, 0, 0.2)
                    }

                    onClicked: statisticsPanel.statisticsExpanded = !statisticsPanel.statisticsExpanded
                    ToolTip.visible: hovered
                    ToolTip.text: statisticsPanel.statisticsExpanded ?
                                      qsTr("Zwiń") : qsTr("Rozwiń statystyki")
                }
            }

            Rectangle {
                anchors.top: headerRow.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                anchors.topMargin: 4
                height: 1
                color: "#494850"
                opacity: 0.7
            }

            ScrollView {
                id: statisticsScroll
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: headerRow.bottom
                anchors.bottom: parent.bottom
                anchors.margins: 8
                anchors.topMargin: 16

                opacity: statisticsPanel.statisticsExpanded ? 1 : 0
                visible: opacity > 0
                Behavior on opacity {
                    NumberAnimation {
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                }

                ColumnLayout {
                    id: statisticsColumn
                    width: statisticsScroll.availableWidth
                             - statisticsScroll.ScrollBar.vertical.width
                             - 8
                    spacing: 8

                    GridLayout {
                        id: statisticsGrid
                        Layout.fillWidth: true
                        rowSpacing: 12
                        columnSpacing: 16
                        readonly property int minCellWidth: 200
                        columns: Math.max(1, Math.min(5, Math.floor(width / minCellWidth)))

                        Repeater {
                            model: simulationHandler ? simulationHandler.statistics : []
                            delegate: StatisticItem {
                                Layout.fillWidth: true
                                stat: modelData
                            }
                        }

                        Item { Layout.fillWidth: true; height: 2 }
                    }
                }
            }
        }

        // --- Simulation Start/Pause/Stop buttons --
        Loader {
            id: controllerView
            anchors.fill: parent
            property var controller: simulationHandler ? simulationHandler.runtimeController : null
            onControllerChanged: {
                if (controller) {
                    setSource(controller.uiSource, {
                                  "controller": controller,
                                  "propertiesExpanded": Qt.binding(function() {
                                      return propertiesPanel.panelExpanded
                                  })})
                } else {
                    source = ""
                }
            }
        }
    }

    // --- Button for menu panel ---
    Button {
        id: openButton
        text: "\uE5D2"
        font.family: materialIcons.name
        font.pixelSize: 24
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        onClicked: {
            if (sidePanel.x !== 0) {
                panelOpen = !panelOpen
                simulationHandler.load()
            }
        }
    }

    // --- Menu panel on the left ---
    Rectangle {
        id: sidePanel
        width: 260
        height: parent.height
        color: Qt.darker(panelColor, 1.15)
        anchors.top: parent.top

        x: panelOpen ? 0 : -width

        Behavior on x {
            NumberAnimation {
                duration: 180
                easing.type: Easing.InOutQuad
            }
        }

        ColumnLayout {
            id: sidePanelColumn
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            Label {
                text: qsTr("Symulacje")
                font.family: "Source Sans 3"
                font.pixelSize: 16
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
                elide: Text.ElideRight
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#494850"
                opacity: 0.7
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: availableWidth

                ColumnLayout {
                    id: simulationsColumn
                    width: parent.width
                    spacing: 4

                    Repeater {
                        model: simulationHandler ? simulationHandler.simulations : []
                        delegate: MenuButton {
                            Layout.fillWidth: true
                            text: modelData.name
                            implicitHeight: 34

                            onClicked: {
                                root.panelOpen = false
                                simulationHandler.select(text)
                                root.simulationSelected = true
                            }
                        }
                    }
                }
            }

            Text {
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.bottomMargin: 14
                font.family: "Source Sans 3"
                font.pixelSize: 10
                color: Qt.lighter(panelColor, 1.4)
                text: "Created by Karol\ngithub.com/Karol03"
            }
        }
    }
}
