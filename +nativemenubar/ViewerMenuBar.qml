import QtQuick
import Qt.labs.platform as Platform

Item {
    required property var application

    Platform.MenuBar {
        Platform.Menu {
            title: qsTr("&File")
            Platform.MenuItem {
                text: qsTr("&Open")
                shortcut: StandardKey.Open
                onTriggered: application.openMeshFileAction()
            }
            Platform.MenuSeparator {}
            Platform.MenuItem {
                text: qsTr("&Quit")
                shortcut: StandardKey.Quit
                onTriggered: application.quitAction()
            }
        }
        Platform.Menu {
            title: qsTr("&Help")
            Platform.MenuItem {
                text: qsTr("&About")
                onTriggered: application.aboutAction()
            }
        }
    }
}
