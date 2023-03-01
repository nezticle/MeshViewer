#ifndef MESHVIEWERAPPLICATION_H
#define MESHVIEWERAPPLICATION_H

#include <QGuiApplication>
#include <QScopedPointer>
#include <QtQml>

class MeshViewerApplication
{
public:
    MeshViewerApplication(int &argc, char **argv, const QString &applicationName);
    ~MeshViewerApplication();

    int run();

    QQmlApplicationEngine *qmlEngine() const;

private:
    QScopedPointer<QGuiApplication> m_application;
    QScopedPointer<QQmlApplicationEngine> m_qmlEngine;
};

#endif // MESHVIEWERAPPLICATION_H
