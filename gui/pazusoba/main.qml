import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Controls.Material 2.12

import org.github.henryquan.bridge 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 480
    height: 640
    minimumWidth: 370
    minimumHeight: 500
    title: qsTr("パズそば - Puzzle & Dragons Solver")

    // Setup theme
    Material.theme: Material.System
    Material.accent: Material.Blue

    // Some properties
    property int gridImageSize: Math.min((Screen.width > window.width ? window.width : Screen.width) / soba.row - 8, 64)

    // Import the C++ bridge
    QBridge {
        id: soba
    }

    onWidthChanged: {
        if (window.width > window.height) {
            // Based on height now
            gridImageSize = window.height / soba.row * 0.618;
        } else {
            // -10 just for extra spacing
            gridImageSize = Math.min((Screen.width > window.width ? window.width : Screen.width) / soba.row - 8, 64);
        }
    }

    // Depend on the screen size, it switches to horizontal or vertical
    GridLayout {
        id: rootGrid
        anchors.fill: parent
        Layout.maximumWidth: 1000
        flow: window.width > window.height ? GridLayout.LeftToRight : GridLayout.TopToBottom

        ColumnLayout {
            id: leftColumn
            Layout.margins: 16
            Layout.alignment: Qt.AlignCenter
            Grid {
                id: boardGrid
                property bool showInitial: true
                property var currentBoard: []
                anchors.margins: 4
                spacing: 4
                // To display properly, you need to swap row and column here
                columns: soba.row
                rows: soba.column

                Repeater {
                    model: soba.initialBoard.length
                    Image {
                        id: gridImage
                        height: gridImageSize
                        width: gridImageSize
                        // Use js to get corresponding image based on index
                        source: {
                            if (boardGrid.showInitial) return `images/${soba.initialBoard[index]}.png`;
                            return `images/${boardGrid.currentBoard[index]}.png`
                        }
                    }
                }
            }

            TextArea {
                id: boardInput
                implicitWidth: boardGrid.width
                placeholderText: qsTr("Paste a board string")
            }

            RowLayout {
                id: solverRow
                width: boardGrid.width
                Layout.alignment: Qt.AlignHCenter
                Text {
                    color: Material.primaryTextColor
                    text: qsTr("Beam")
                }
                ComboBox {
                    id: beamBox
                    currentIndex: 2
                    onCurrentIndexChanged: {
                        soba.updateBeam(currentIndex);
                    }
                    model: soba.beamList
                }
                Text {
                    color: Material.primaryTextColor
                    text: qsTr("Step")
                }
                ComboBox {
                    id: stepBox
                    currentIndex: 1
                    onCurrentIndexChanged: {
                        soba.updateStep(currentIndex);
                    }
                    model: soba.stepList
                }
            }

            RowLayout {
                id: buttonRow
                Layout.alignment: Qt.AlignHCenter
                Button {
                    text: "Random"
                    onClicked: {
                        soba.random();
                        // Set show initial again
                        boardGrid.showInitial = true;
                    }
                }
                Button {
                    text: "Reset"
                    onClicked: {
                        boardGrid.showInitial = true;
                    }
                }
                Button {
                    text: "Solve"
                    onClicked: {
                        soba.solve();
                    }
                }
            }
        }

        ListView {
            id: routeList
            Layout.margins: 16
            Layout.fillHeight: true
            Layout.fillWidth: true
            ScrollBar.vertical: ScrollBar {}
            model: soba.routes
            delegate: MouseArea {
                height: 64
                width: routeList.width
                onClicked: {
                    boardGrid.currentBoard = modelData["board"];
                    boardGrid.showInitial = false;
                }

                Text {
                    wrapMode: Text.Wrap
                    color: Material.primaryTextColor
                    text: qsTr(modelData['info'])
                }
            }
        }
    }
}
