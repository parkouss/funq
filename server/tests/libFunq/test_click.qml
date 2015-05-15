import QtQuick 2.0

Item {

    width: 320
    height: 480

    Rectangle {
        id: rectangle
        color: "#272822"
        width: 320
        height: 480
        MouseArea {
            anchors.fill: parent
            onClicked: rectangle.color = "#FFFFFF"
        }
    }

    
}
