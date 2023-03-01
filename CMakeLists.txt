cmake_minimum_required(VERSION 3.16)
project(MeshViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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
    meshviewerapplication.h meshviewerapplication.cpp
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

qt_add_qml_module(MeshViewer
    URI "MeshViewer"
    VERSION "${PROJECT_VERSION}"
    QML_FILES
        main.qml
        UvCheckerMaterial.qml
        VertexColorMaterial.qml
        ViewerMenuBar.qml
        +nativemenubar/ViewerMenuBar.qml
    RESOURCES
        UVCheckerMap01.png
        UVChecker.frag
        UVChecker.vert
        VertexColor.frag
        VertexColor.vert
    NO_RESOURCE_TARGET_PATH
)
