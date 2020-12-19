/*
 * Copyright (c) 2020 Andy Nichols <nezticle@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
