import QtQuick 2.0

Item {
    id: main
    Text {
        text: "Parent"
    }

    Item {
        Text {
            text: "Child"
        }

        Item {
            Text {
                text: "Grandchild"
            }
        }
    }
}
