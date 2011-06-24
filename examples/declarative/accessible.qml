import QtQuick 1.0

Rectangle {
    width: 200
    height: 200

    Text {
        text: "Hello World!"
        Accessible.role: Accessible.StaticText

        focus: true

        anchors.right: parent.right

        scale: 0.5
        rotation: 90
    }

    Button {
        text: "Click me!"
        x: 20
        y: 150
    }

    TextInput {
        text: "Input..."
        focus: true
        Accessible.role: Accessible.StaticText

        Rectangle {
            anchors.centerIn: parent
            height: parent.height + 8
            width: parent.width + 8
            opacity: 0.1
            color: "blue"
            radius: 4
            visible: parent.focus == true
        }
    }
}
