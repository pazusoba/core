import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("パズそば - Puzzle & Dragons Solver")

    Grid {
        id: grid
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        columns: 6
        rows: 5
        spacing: 4
        Repeater {
            id: repeater
            model: 30
            Rectangle { color: "red"; radius: 25; width: 50; height: 50 }
        }
    }
}
