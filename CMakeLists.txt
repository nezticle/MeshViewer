cmake_minimum_required(VERSION 3.14)
project(MeshViewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Quick3D)
find_package(Qt6 COMPONENTS Widgets)

qt_add_executable(MeshViewer
    colordialoghelper.cpp colordialoghelper.h
    filedialoghelper.cpp filedialoghelper.h
    geometrygenerator.cpp geometrygenerator.h
    mesh.cpp mesh.h
    meshinfo.cpp meshinfo.h
    subsetdatatablemodel.cpp subsetdatatablemodel.h
    subsetlistmodel.cpp subsetlistmodel.h
    main.cpp
)
set_target_properties(MeshViewer PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(MeshViewer PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Quick3D
    Qt::Widgets
)

# Resources:
set(qml_resource_files
    "main.qml"
    "UvCheckerMaterial.qml"
    "UVCheckerMap01.png"
    "UVChecker.frag"
    "UVChecker.vert"
    "VertexColorMaterial.qml"
    "VertexColor.frag"
    "VertexColor.vert"
)

qt6_add_resources(MeshViewer "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

set_target_properties(MeshViewer PROPERTIES
    QT_QML_MODULE_VERSION 1.0
    QT_QML_MODULE_URI MeshViewer
)

qt6_qml_type_registration(MeshViewer)