import QtQuick 2.0


FocusScope {
    property alias text: label.text
    accessibleRole: Qt.PushButton

    id: scope

    width: 100
    height: 62

    Rectangle {
        visible: scope.focus == true
        anchors.centerIn: parent
        height: parent.height + 8
        width: parent.width + 8
        opacity: 0.1
        color: "blue"
        radius: 6
    }

    Rectangle {
        id: button
        anchors.fill: parent
        radius: 4

        color: "#330033"
        border.color: "black"

        Text {
            id: label
            anchors.centerIn: parent
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                scope.focus = true;
                if (button.color == "#330033") {
                    button.color = "#660066"
                } else {
                    button.color = "#330033"
                }
            }
        }
        Keys.onPressed: {
            if (event.key == Qt.Key_1)
                label.text = 'Key 1'
        }
    }
}
