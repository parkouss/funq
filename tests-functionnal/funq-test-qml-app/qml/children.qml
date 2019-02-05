import QtQuick 2.0

Rectangle {
    id: main
    width: 600
    height: 600
    color: "red"

    Text {
        text: "Parent"
    }

    Rectangle {
        width: 400
        height: 400
        color: "green"
        anchors.centerIn: parent
        Text {
            text: "Child"
        }

        Rectangle {
            width: 200
            height: 200
            color: "blue"
            anchors.centerIn: parent
            Text {
                text: "Grandchild"
            }
        }
    }
}
