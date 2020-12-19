#include "meshinfo.h"

#include <QtQml/QQmlFile>
#include <QtQml/QQmlContext>

MeshInfo::MeshInfo(QObject *parent) : QObject(parent)
{
    m_subsetListModel = new SubsetListModel();
    m_subsetDataTableModel = new SubsetDataTableModel();
}

MeshInfo::~MeshInfo()
{
    delete m_subsetListModel;
    delete m_subsetDataTableModel;
    if (!m_meshes.isEmpty())
        qDeleteAll(m_meshes);
}

QUrl MeshInfo::meshFile() const
{
    return m_meshFile;
}

SubsetListModel *MeshInfo::subsetListModel() const
{
    return m_subsetListModel;
}

SubsetDataTableModel *MeshInfo::subsetDataTableModel() const
{
    return m_subsetDataTableModel;
}

void MeshInfo::setMeshFile(QUrl meshFile)
{
    if (m_meshFile == meshFile)
        return;

    m_meshFile = meshFile;
    emit meshFileChanged(m_meshFile);
    updateSourceMeshFile();
}

void MeshInfo::updateSourceMeshFile()
{
    const QQmlContext *context = qmlContext(this);
    const QString meshPath = QQmlFile::urlToLocalFileOrQrc(context ? context->resolvedUrl(m_meshFile) : m_meshFile);

    // Cleanup
    m_subsetListModel->setMesh(nullptr);
    m_subsetDataTableModel->setMesh(nullptr);

    if (!m_meshes.isEmpty())
        qDeleteAll(m_meshes);

    m_meshes = m_meshFileTool.loadMeshFile(meshPath);

    if (!m_meshes.isEmpty()) {
        m_subsetListModel->setMesh(m_meshes.first());
        m_subsetDataTableModel->setMesh(m_meshes.first());
    }
}
