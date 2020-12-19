#ifndef FILEDIALOGHELPER_H
#define FILEDIALOGHELPER_H

#include <QObject>
#include <QUrl>
#include <qqml.h>

class FileDialogHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl fileName READ fileName NOTIFY fileNameChanged)
    QML_ELEMENT
public:
    explicit FileDialogHelper(QObject *parent = nullptr);

    QUrl fileName() const;

    Q_INVOKABLE void openFile();

signals:
    void fileNameChanged(QUrl fileName);

private:
    QUrl m_fileName;
};

#endif // FILEDIALOGHELPER_H
