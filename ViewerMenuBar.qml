import QtQuick
import QtQuick.Controls

MenuBar {
    required property var application
    Menu {
        title: qsTr("&File")
        Action {
            id: openMeshFileAction
            text: qsTr("&Open")
            shortcut: StandardKey.Open
            onTriggered: application.openMeshFileAction()
        }
        MenuSeparator {}
        Action {
            id: quitAction
            text: qsTr("&Quit")
            shortcut: StandardKey.Quit
            onTriggered: application.quitAction()
        }
    }
    Menu {
        title: qsTr("&Help")
        Action {
            text: qsTr("&About")
            onTriggered: application.aboutAction()
        }
    }
}
