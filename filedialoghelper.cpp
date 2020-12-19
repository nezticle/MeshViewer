#include "filedialoghelper.h"
#include <QFileDialog>

FileDialogHelper::FileDialogHelper(QObject *parent) : QObject(parent)
{

}

QUrl FileDialogHelper::fileName() const
{
    return m_fileName;
}

void FileDialogHelper::openFile()
{
    const QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                          "Open Mesh File",
                                                          QString(),
                                                          "Mesh (*.mesh)");
    m_fileName = QUrl::fromLocalFile(fileName);
    emit fileNameChanged(m_fileName);
}
