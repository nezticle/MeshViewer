QT += quick quick3d widgets

CONFIG += qmltypes
QML_IMPORT_NAME = MeshViewer
QML_IMPORT_MAJOR_VERSION = 1

HEADERS += \
    filedialoghelper.h \
    mesh.h \
    meshinfo.h \
    subsetdatatablemodel.h \
    subsetlistmodel.h

SOURCES += \
    filedialoghelper.cpp \
    main.cpp \
    mesh.cpp \
    meshinfo.cpp \
    subsetdatatablemodel.cpp \
    subsetlistmodel.cpp

RESOURCES += \
    qml.qrc
