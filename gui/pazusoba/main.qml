import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import org.github.henryquan.bridge 1.0

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("パズそば - Puzzle & Dragons Solver")

    // Make sure you actually use it or it never works
    Bridge {
        id: soba
    }

    onVisibleChanged: {
        // This used for basic testing
        console.log(soba.initialBoard)
    }

    Column {
        // Put column in the middle
        anchors.horizontalCenter: parent.horizontalCenter
        Grid {
            id: grid
            // To display properly, you need to swap row and column here
            columns: soba.row
            rows: soba.column
            spacing: 4
            Repeater {
                model: soba.initialBoard.length
                Image {
                    // Use js to get corresponding image based on index
                    source: `images/${soba.initialBoard[index]}.png`
                }
            }
        }

        ListView {

            width: 200; height: 200
            ScrollBar.vertical: ScrollBar {}
            model: soba.initialBoard.length
            delegate: Text {
                text: `${index} index +++++++++++++++`
            }
        }
    }
}
