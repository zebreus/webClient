import QtQuick 2.13
import QtQuick.Controls 2.13
import webclient.certificategeneratorbackend 1.0

Page {
    id: page
    property alias hostname: hostname
    property alias wssBox: wssBox
    property alias spinBox: spinBox
    property alias openButton: openButton
    property alias generateButton: generateButton
    property alias stateLabel: stateLabel
    property alias fileListView: fileListView
    title: qsTr("Certificate Generator")
    scale: 1
    transformOrigin: Item.Center

    Column {
        id: column
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10

        Row {
            id: row
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5

            Label {
                id: label
                text: qsTr("STATE: ")
            }

            Label {
                id: stateLabel
            }
        }

        TextField {
            id: hostname
            width: 250
            placeholderText: qsTr("Hostname")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CheckBox {
            id: wssBox
            text: qsTr("Use wss")
            tristate: false
            anchors.horizontalCenter: parent.horizontalCenter
        }

        SpinBox {
            id: spinBox
            editable: true
            to: 65535
            from: 1
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            id: openButton
            text: qsTr("Open")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            id: generateButton
            text: qsTr("Generate")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label1
            text: qsTr("Generated certificates:")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ListView {
            id: fileListView
            width: 500
            height: 160
            clip: true
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
