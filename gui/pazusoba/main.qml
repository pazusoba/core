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

    Grid {
        id: grid
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        columns: soba.column
        rows: soba.row
        spacing: 4
        Repeater {
            id: repeater
            model: soba.column * soba.row
            Image {
                id: orbImage
                source: "images/1.png"
            }
        }
    }
}
