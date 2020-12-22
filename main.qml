/*
 * Copyright (c) 2020 Andy Nichols <nezticle@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Controls
import QtQuick.Layouts
import MeshViewer

ApplicationWindow {
    width: 1280
    height: 720
    title: qsTr("Mesh File Viewer")
    visible: true

    FileDialogHelper {
        id: fileDialogHelper
    }

    Action {
        id: openMeshFileAction
        text: qsTr("&Open")
        shortcut: StandardKey.Open
        onTriggered: {
            fileDialogHelper.openFile();
        }
    }
    Action {
        id: quitAction
        text: qsTr("&Quit")
        shortcut: StandardKey.Quit
        onTriggered: {
            Qt.quit();
        }
    }
    Action {
        id: aboutAction
        text: qsTr("&About")
    }


    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                action: openMeshFileAction
            }
            MenuItem {
                action: quitAction
            }
        }
        Menu {
            title: qsTr("&Help")
            MenuItem {
                action: aboutAction
            }
        }
    }

    MeshInfo {
        id: meshInfo
        meshFile: fileDialogHelper.fileName
    }

    Item {
        anchors.fill: parent

        ListView {
            id: listView
            model: meshInfo.subsetListModel
            width: 100
            height: 200
            anchors.top: parent.top
            anchors.left: parent.left
            clip: true
            currentIndex: 0
            delegate: Item {
                width: 100
                height: 25
                //                color: "white"

                Text {
                    anchors.centerIn: parent
                    text: subsetName
                    font.pixelSize: 16
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        listView.currentIndex = index
                    }
                }
            }
            focus: true
            highlight: Rectangle {
                color: "green"
                width: 100
                height: 25
            }
            header: Rectangle {
                width: 100
                height: 25
                color: "#333333"
                Text {
                    anchors.centerIn: parent
                    color: '#aaaaaa'
                    text: "Subset"
                    font.pixelSize: 16
                }
            }

            onCurrentIndexChanged: {
                meshInfo.subsetDataTableModel.subsetIndex = currentIndex;
                console.log("current index: " + currentIndex)
            }
        }
        TableView {
            id: tableView
            model: meshInfo.subsetDataTableModel
            height: 200
            anchors.top: parent.top
            anchors.left: listView.right
            anchors.right: parent.right
            topMargin: 25
            clip: true

            columnWidthProvider: function (column) {
                if (column === 0)
                    return 50;
                return 200;
            }

            // Column Header
            Row {
                id: columnsHeader
                y: tableView.contentY
                z: 2
                Repeater {
                    model: tableView.columns > 0 ? tableView.columns : 1
                    Label {
                        width: tableView.columnWidthProvider(modelData)
                        height: 25
                        text: meshInfo.subsetDataTableModel.headerData(modelData, Qt.Horizontal)
                        color: '#aaaaaa'
                        font.pixelSize: 16
                        padding: 10
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter

                        background: Rectangle { color: "#333333" }
                    }
                }
            }

            delegate: Rectangle {
                implicitWidth: 200
                implicitHeight: 25
                color: "white"
                border.width: 1
                Text {
                    anchors.centerIn: parent
                    text: display
                }
            }
        }

        Item {
            id: viewportContainer
            anchors.top: tableView.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            Item {
                id: settingsContainer
                width: 300
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                Flickable {
                    anchors.fill: parent
                    contentHeight: settingsLayout.height
                    clip: true
                    ColumnLayout {
                        id: settingsLayout
                        width: parent.width
                        Label {
                            text: "3D View Settings"
                            font.pointSize: 24
                            Layout.alignment: Qt.AlignHCenter
                        }
                        CheckBox {
                            id: axisHelpersCheckBox
                            checked: true
                            text: "Show Axis Helpers"
                        }
                        GroupBox {
                            title: "Model Transform"
                            Layout.fillWidth: true;
                            ColumnLayout {
                                anchors.fill: parent
                                RowLayout {
                                    Label {
                                        text: "Scale"
                                    }
                                    Slider {
                                        id: scaleSlider
                                        from: 0.01
                                        to: 100
                                        value: 1.0
                                    }
                                }
                                RowLayout {
                                    Label {
                                        text: "Rotation X"
                                    }
                                    Slider {
                                        id: rotationXSlider
                                        from: 0
                                        to: 360
                                        value: 0
                                    }
                                }
                                RowLayout {
                                    Label {
                                        text: "Rotation Y"
                                    }
                                    Slider {
                                        id: rotationYSlider
                                        from: 0
                                        to: 360
                                        value: 0
                                    }
                                }
                                RowLayout {
                                    Label {
                                        text: "Rotation Z"
                                    }
                                    Slider {
                                        id: rotationZSlider
                                        from: 0
                                        to: 360
                                        value: 0
                                    }
                                }
                            }
                        }
                        GroupBox {
                            title: "Views"
                            Layout.fillWidth: true;
                            ColumnLayout {
                                anchors.fill: parent
                                CheckBox {
                                    id: originalViewCheckBox
                                    checked: true
                                    text: "Original"
                                }
                                CheckBox {
                                    id: wireframeViewCheckBox
                                    checked: false
                                    text: "Wireframe"
                                }
                                CheckBox {
                                    id: normalsViewCheckBox
                                    checked: false
                                    text: "Normals"
                                }
                                CheckBox {
                                    id: tangentsViewCheckBox
                                    checked: false
                                    text: "Tangents"
                                }
                                CheckBox {
                                    id: binormalsViewCheckBox
                                    checked: false
                                    text: "Binormals"
                                }
                                CheckBox {
                                    id: uvViewCheckBox
                                    checked: false
                                    text: "UV0"
                                }

                                CheckBox {
                                    id: colorViewCheckBox
                                    checked: false
                                    text: "Vertex Colors"
                                }

                            }
                        }
                    }
                }
            }

            View3D {
                id: view3D
                anchors.top: parent.top
                anchors.left: settingsContainer.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                environment: SceneEnvironment {
                    clearColor: "black"
                    backgroundMode: SceneEnvironment.Color
                }

                PerspectiveCamera {
                    id: camera
                    z: 200
                }

                DirectionalLight {
                    ambientColor: Qt.rgba(0.3, 0.3, 0.3, 1.0)
                }
                Node {
                    id: modelContainer
                    scale: Qt.vector3d(scaleSlider.value, scaleSlider.value, scaleSlider.value)
                    eulerRotation: Qt.vector3d(rotationXSlider.value, rotationYSlider.value, rotationZSlider.value)
                    Model {
                        id: originalModel
                        visible: geometryGenerator.original !== null && originalViewCheckBox.checked
                        geometry: geometryGenerator.original
                        materials: PrincipledMaterial {
                        }
                    }
                    Model {
                        id: wireframeModel
                        visible: geometryGenerator.wireframe !== null && wireframeViewCheckBox.checked
                        geometry: geometryGenerator.wireframe
                        materials: PrincipledMaterial {
                            baseColor: "magenta"
                            cullMode: Material.NoCulling
                            lighting: PrincipledMaterial.NoLighting
                        }
                    }

                    Model {
                        id: normalsModel
                        visible: geometryGenerator.normals !== null && normalsViewCheckBox.checked
                        geometry: geometryGenerator.normals
                        materials: PrincipledMaterial {
                            baseColor: "blue"
                            cullMode: Material.NoCulling
                            lighting: PrincipledMaterial.NoLighting
                        }
                    }
                    Model {
                        id: tangentsModel
                        visible: geometryGenerator.tangents !== null && tangentsViewCheckBox.checked
                        geometry: geometryGenerator.tangents
                        materials: PrincipledMaterial {
                            baseColor: "red"
                            cullMode: Material.NoCulling
                            lighting: PrincipledMaterial.NoLighting
                        }
                    }
                    Model {
                        id: binormalsModel
                        visible: geometryGenerator.binormals !== null && binormalsViewCheckBox.checked
                        geometry: geometryGenerator.binormals
                        materials: PrincipledMaterial {
                            baseColor: "green"
                            cullMode: Material.NoCulling
                            lighting: PrincipledMaterial.NoLighting
                        }
                    }

                    Model {
                        id: uvCheckerModel
                        visible: geometryGenerator.original !== null && uvViewCheckBox.checked
                        geometry: geometryGenerator.original
                        materials: UvCheckerMaterial {

                        }
                    }
                    Model {
                        id: vertexColorModel
                        visible: geometryGenerator.original !== null && colorViewCheckBox.checked
                        geometry: geometryGenerator.original
                        materials: VertexColorMaterial {

                        }
                    }
                }

                GeometryGenerator {
                    id: geometryGenerator
                    meshInfo: meshInfo
                    subsetIndex: listView.currentIndex
                }

                AxisHelper {
                    visible: axisHelpersCheckBox.checked
                }

                MouseArea {
                    id: view3DMouseArea
                    property bool isInputActive: false
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        isInputActive = true
                        wasdController.focus = true
                    }
                    onExited: {
                        isInputActive = false
                        wasdController.focus = false
                    }
                    onPressed: {
                        wasdController.focus = true
                        wasdController.keysEnabled = true
                    }
                }
            }

            WasdController {
                id: wasdController
                controlledObject: camera
                mouseEnabled: view3DMouseArea.isInputActive
                keysEnabled: view3DMouseArea.isInputActive
            }

        }
    }
}
