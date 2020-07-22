import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import org.github.henryquan.bridge 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 480
    height: 640
    title: qsTr("パズそば - Puzzle & Dragons Solver")

    property int gridImageSize: Math.min(window.width / soba.row - 8, 64)

    // Make sure you actually use it or it never works
    Bridge {
        id: soba
    }

    onWidthChanged: {
        // -6 just for extra spacing
        gridImageSize = Math.min(window.width / soba.row - 8, 64)
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 20
        rowSpacing: 20
        columnSpacing: 20
        flow:  width > height ? GridLayout.LeftToRight : GridLayout.TopToBottom

        GridLayout {
            id: board
            Layout.alignment: Qt.AlignHCenter
            Layout.margins: 4
            Layout.preferredWidth: 100
            // To display properly, you need to swap row and column here
            columns: soba.row
            rows: soba.column

            Repeater {
                model: soba.initialBoard.length
                Image {
                    // Use js to get corresponding image based on index
                    source: `images/${soba.initialBoard[index]}.png`
                }
            }
        }

        ListView {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillHeight: true
            Layout.bottomMargin: 16
            ScrollBar.vertical: ScrollBar {}
            model: soba.initialBoard.length
            delegate: Text {
                text: `${index} index +++++++++++++++`
            }
        }
    }
}
