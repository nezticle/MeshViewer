#include "meshviewerapplication.h"

#include <QtGui/QFontDatabase>

#include <QQmlContext>
#include <QQmlApplicationEngine>

#include <QtQuick3D/qquick3d.h>

Q_LOGGING_CATEGORY(lcApp, "meshviewer.application")

static QGuiApplication *createApplication(int &argc, char **argv, const QString &applicationName)
{
    QGuiApplication::setOrganizationName("The Qt Company");
    QGuiApplication::setApplicationName(applicationName);
    QGuiApplication::setOrganizationDomain("qt.io");
    QGuiApplication::setApplicationDisplayName("Mesh Viewer");
    QGuiApplication::setApplicationVersion("0.1");

    QGuiApplication *app = new QGuiApplication(argc, argv);
    return app;
}

MeshViewerApplication::MeshViewerApplication(int &argc, char **argv, const QString &applicationName)
    : m_application(createApplication(argc, argv, applicationName))
    , m_qmlEngine(new QQmlApplicationEngine)
{
    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());

    // Extra File Selectors for native features
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    QQmlFileSelector fileSelector(m_qmlEngine.data());
    fileSelector.setExtraSelectors(QStringList() << QLatin1String("nativemenubar"));
#endif

    // Load QML
    qCDebug(lcApp) << "Loading main.qml...";
    m_qmlEngine->load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (!m_qmlEngine->rootObjects().isEmpty()) {
        qCDebug(lcApp) << "... loaded main.qml";
    } else {
        qCDebug(lcApp) << "... failed to load main.qml";
    }

    qCDebug(lcApp) << "... constructed MeshViewerApplication";
}

MeshViewerApplication::~MeshViewerApplication()
{
    m_qmlEngine.reset();
}

int MeshViewerApplication::run()
{
    return m_application->exec();
}

QQmlApplicationEngine *MeshViewerApplication::qmlEngine() const
{
    return m_qmlEngine.data();
}
