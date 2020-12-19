#ifndef SUBSETLISTMODEL_H
#define SUBSETLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <qqml.h>

#include "mesh.h"

class SubsetListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Created by MeshImage")
public:
    enum SubsetListModelRoles {
        SubsetName = Qt::UserRole + 1
    };


    SubsetListModel();
    Mesh* mesh() const;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void setMesh(Mesh* mesh);

signals:
    void meshChanged(Mesh* mesh);

private:
    Mesh *m_mesh = nullptr;

};

#endif // SUBSETLISTMODEL_H
