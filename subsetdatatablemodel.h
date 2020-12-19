#ifndef SUBSETDATATABLEMODEL_H
#define SUBSETDATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <qqml.h>
#include "mesh.h"

class SubsetDataTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int subsetIndex READ subsetIndex WRITE setSubsetIndex NOTIFY subsetIndexChanged)
    QML_ELEMENT
    QML_UNCREATABLE("Created by MeshImage")
public:  
    SubsetDataTableModel();
    Mesh* mesh() const;
    int subsetIndex() const;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void setMesh(Mesh* mesh);
    void setSubsetIndex(int subsetIndex);

signals:
    void meshChanged(Mesh* mesh);
    void subsetIndexChanged(int subsetIndex);

private:
    struct AttributeField {
        enum Attribute{
            Position,
            UV,
            Normal,
            Tangent,
            Binormal,
            Color,
            Joint,
            Weight,
            MorphTargetPosition,
            MorphTargetNormal,
            MorphTargetTangent,
            MorphTargetBinormal
        };
        QString headerName;
        Attribute attribute;
        int index = 0;
        AttributeField(QString name, Attribute attr, int id = 0)
            : headerName(name)
            , attribute(attr)
            , index(id)
        {}
    };

    void updateModelData();
    QVector<AttributeField> m_fields;
    Mesh *m_mesh = nullptr;
    int m_subsetIndex = 0;   
};

#endif // SUBSETDATATABLEMODEL_H
