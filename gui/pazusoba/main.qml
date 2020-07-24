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
        // -10 just for extra spacing
        gridImageSize = Math.min((Screen.width > window.width ? window.width : Screen.width) / soba.row - 8, 64);
    }

    // Depend on the screen size, it switches to horizontal or vertical
    GridLayout {
        id: rootGrid
        flow: window.width > window.height ? GridLayout.LeftToRight : GridLayout.TopToBottom

        ColumnLayout {
            id: leftColumn
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

            RowLayout {
                id: buttonRow
                Button {
                    text: "Solve"
                    onClicked: {
                        soba.solve();
                    }
                }
                Button {
                    text: "Routes"
                }
                Button {
                    text: "Reset"
                    onClicked: {
                        boardGrid.showInitial = true;
                    }
                }
            }
        }

        ColumnLayout {
            id: rightColumn
            ListView {
                id: routeList
                Layout.fillHeight: true
                Layout.fillWidth: true
                ScrollBar.vertical: ScrollBar {}
                model: soba.routes
                delegate: MouseArea {
                    height: 48
                    width: 100
                    onClicked: {
                        boardGrid.currentBoard = modelData["board"];
                        boardGrid.showInitial = false;
                    }

                    Text {
                        color: Material.primaryTextColor
                        text: qsTr(modelData['info'])
                    }
                }
            }
        }
    }
}
