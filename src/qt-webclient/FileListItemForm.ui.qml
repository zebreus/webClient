import QtQuick 2.4
import QtQuick.Controls 2.13

Item {
    height: 25
    property alias itemButton: itemButton
    width: 500

    Button {
        id: itemButton
        text: fileListDelegate.text
        anchors.fill: parent
    }
}
