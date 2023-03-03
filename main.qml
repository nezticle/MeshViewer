/*
 * Copyright (c) 2023 Andy Nichols <nezticle@gmail.com>
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
import QtCore
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import MeshViewer

ApplicationWindow {
    id: appWindow
    width: 1280
    height: 720
    title: qsTr("Mesh File Viewer") + " " + meshInfo.meshName
    visible: true

    FileDialog {
        id: openMeshFileDialog
        fileMode: FileDialog.OpenFile
        currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)
        nameFilters: ["Mesh file (*.mesh)"]
        onAccepted: {
            meshInfo.meshFile = openMeshFileDialog.currentFile
        }
    }

    function openMeshFileAction() {
        openMeshFileDialog.open();
    }

    function quitAction() {
        Qt.quit();
    }

    function aboutAction() {

    }


    menuBar: ViewerMenuBar {
        application: appWindow
    }

    MeshInfo {
        id: meshInfo
    }

    Pane {
        anchors.fill: parent

        RowLayout {
            id: meshDataRow
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            ListView {
                id: listView
                model: meshInfo.subsetListModel
                width: 100
                height: 200

                clip: true
                currentIndex: 0
                ScrollIndicator.vertical: ScrollIndicator {}
                delegate: ItemDelegate {
                    text: subsetName === "" ? index : subsetName
                    width: parent.width
                    onClicked: {
                        listView.currentIndex = index
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
            Item {
                // Item to work around the Pane in the butt...
                height: 200
                Layout.fillWidth: true


                Pane {
                    id: tableViewContainer
                    anchors.fill: parent

                    TableView {
                        id: tableView
                        model: meshInfo.subsetDataTableModel
                        anchors.fill: parent
                        anchors.topMargin: horizontalHeader.height + rowSpacing
                        anchors.leftMargin: verticalHeader.width + columnSpacing
                        columnSpacing: 1
                        rowSpacing: 1
                        syncDirection: Qt.Vertical | Qt.Horizontal
                        implicitWidth: parent.width + columnSpacing
                        implicitHeight: parent.height + rowSpacing

                        clip: true

                        Connections {
                            target: meshInfo.subsetDataTableModel
                            function onModelReset() {
                                tableView.selectedRow = -1;
                            }
                        }

                        property int selectedRow: -1
                        delegate: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 25
                            color: row === tableView.selectedRow ? "green" : tableView.palette.base
                            border.width: 1
                            Label {
                                anchors.centerIn: parent
                                text: display
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (tableView.selectedRow != row)
                                        tableView.selectedRow = row
                                    else
                                        tableView.selectedRow = -1
                                }
                            }
                        }
                    }

                    HorizontalHeaderView {
                        id: horizontalHeader
                        anchors.top: parent.top
                        anchors.left: tableView.left
                        syncView: tableView
                        clip: true
                    }

                    VerticalHeaderView {
                        id: verticalHeader
                        anchors.top: tableView.top
                        syncView: tableView
                        clip: true
                    }
                }
            }
        }

        Item {
            id: viewportContainer
            anchors.top: meshDataRow.bottom
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
                    ScrollBar.vertical: ScrollBar { }
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
                                ColumnLayout {

                                    Label {
                                        text: "Scale"
                                    }
                                    RowLayout {
                                        Slider {
                                            id: scaleSlider
                                            from: 0.01
                                            to: 100
                                            value: 1.0
                                            Layout.fillWidth: true
                                        }
                                        TextField {
                                            text: scaleSlider.value
                                            maximumLength: 5
                                            Layout.preferredWidth: 50
                                            validator: DoubleValidator {
                                                bottom: scaleSlider.from
                                                top: scaleSlider.to
                                                decimals: 2
                                            }
                                            onAccepted: {
                                                scaleSlider.value = text
                                            }
                                        }
                                    }
                                }
                                ColumnLayout {
                                    Label {
                                        text: "Rotation X"
                                    }
                                    RowLayout {
                                        Slider {
                                            id: rotationXSlider
                                            from: 0
                                            to: 360
                                            value: 0
                                            Layout.fillWidth: true
                                        }
                                        TextField {
                                            text: rotationXSlider.value
                                            maximumLength: 5
                                            Layout.preferredWidth: 50
                                            validator: DoubleValidator {
                                                bottom: rotationXSlider.from
                                                top: rotationXSlider.to
                                                decimals: 2
                                            }
                                            onAccepted: {
                                                rotationXSlider.value = text
                                            }
                                        }
                                    }
                                }
                                ColumnLayout {
                                    Label {
                                        text: "Rotation Y"
                                    }
                                    RowLayout {
                                        Slider {
                                            id: rotationYSlider
                                            from: 0
                                            to: 360
                                            value: 0
                                            width: 100
                                            Layout.fillWidth: true
                                        }
                                        TextField {
                                            text: rotationYSlider.value
                                            maximumLength: 5
                                            Layout.preferredWidth: 50
                                            validator: DoubleValidator {
                                                bottom: rotationYSlider.from
                                                top: rotationYSlider.to
                                                decimals: 2
                                            }
                                            onAccepted: {
                                                rotationYSlider.value = text
                                            }
                                        }
                                    }
                                }
                                ColumnLayout {
                                    width: parent.width
                                    Label {
                                        text: "Rotation Z"
                                    }
                                    RowLayout {
                                        Slider {
                                            id: rotationZSlider
                                            from: 0
                                            to: 360
                                            value: 0
                                            Layout.fillWidth: true
                                        }
                                        TextField {
                                            text: rotationZSlider.value
                                            maximumLength: 5
                                            Layout.preferredWidth: 50
                                            validator: DoubleValidator {
                                                bottom: rotationZSlider.from
                                                top: rotationZSlider.to
                                                decimals: 2
                                            }
                                            onAccepted: {
                                                rotationZSlider.value = text
                                            }
                                        }
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
                                    enabled: geometryGenerator.normals !== null
                                    checked: false
                                    text: "Normals"
                                }
                                CheckBox {
                                    id: tangentsViewCheckBox
                                    enabled: geometryGenerator.tangents !== null
                                    checked: false
                                    text: "Tangents"
                                }
                                CheckBox {
                                    id: binormalsViewCheckBox
                                    enabled: geometryGenerator.binormals !== null
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
                environment: ExtendedSceneEnvironment {
                    backgroundMode: SceneEnvironment.SkyBox
                    antialiasingQuality: SceneEnvironment.High
                    antialiasingMode: SceneEnvironment.MSAA
                    lightProbe: Texture {
                        textureData: ProceduralSkyTextureData {

                        }
                    }
                    skyboxBlurAmount: 0.5
                    ditheringEnabled: true
                    InfiniteGrid {
                        id: infiniteGrid
                        visible: axisHelpersCheckBox.checked
                    }
                }

                Node {
                    id: originNode
                    eulerRotation.x: -30
                    PerspectiveCamera {
                        id: cameraNode
                        z: 200
                    }
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
                            baseColor: "grey"
                            metalness: 0.0
                            roughness: 0.3
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
                    Model {
                        id: vertexSelectionModel
                        visible: tableView.selectedRow !== -1
                        source: "#Sphere"
                        scale: Qt.vector3d(0.01 * geometryGenerator.scaleFactor,
                                           0.01 * geometryGenerator.scaleFactor,
                                           0.01 * geometryGenerator.scaleFactor)
                        materials: PrincipledMaterial {
                            baseColor: "yellow"
                            lighting: PrincipledMaterial.NoLighting
                        }
                        position: meshInfo.subsetDataTableModel.vertexPositionAtRow(tableView.selectedRow)
                    }
                }

                GeometryGenerator {
                    id: geometryGenerator
                    meshInfo: meshInfo
                    subsetIndex: listView.currentIndex
                }
                OrbitCameraController {
                    id: cameraController
                    camera: cameraNode
                    origin: originNode
                }

                DebugView {
                    source: view3D
                    anchors.top: parent.top
                    anchors.right: parent.right
                }
            }
        }
    }
}
