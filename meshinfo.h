#ifndef MESHINFO_H
#define MESHINFO_H

#include <QObject>
#include <qqml.h>

#include "subsetlistmodel.h"
#include "subsetdatatablemodel.h"
#include "mesh.h"

class MeshInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl meshFile READ meshFile WRITE setMeshFile NOTIFY meshFileChanged)
    Q_PROPERTY(SubsetListModel* subsetListModel READ subsetListModel NOTIFY subsetListModelChanged)
    Q_PROPERTY(SubsetDataTableModel* subsetDataTableModel READ subsetDataTableModel NOTIFY subsetDataTableModelChanged)
    QML_ELEMENT
public:
    explicit MeshInfo(QObject *parent = nullptr);
    ~MeshInfo();

    QUrl meshFile() const;
    SubsetListModel* subsetListModel() const;
    SubsetDataTableModel* subsetDataTableModel() const;

public slots:
    void setMeshFile(QUrl meshFile);

signals:
    void meshFileChanged(QUrl meshFile);
    void subsetListModelChanged(SubsetListModel* subsetListModel);
    void subsetDataTableModelChanged(SubsetDataTableModel* subsetDataTableModel);

private:
    void updateSourceMeshFile();
    QUrl m_meshFile;
    SubsetListModel* m_subsetListModel = nullptr;
    SubsetDataTableModel* m_subsetDataTableModel = nullptr;
    QVector<Mesh *> m_meshes;
    MeshFileTool m_meshFileTool;
};

#endif // MESHINFO_H
