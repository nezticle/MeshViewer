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

#include "subsetdatatablemodel.h"

SubsetDataTableModel::SubsetDataTableModel()
{

}

Mesh *SubsetDataTableModel::mesh() const
{
    return m_mesh;
}

void SubsetDataTableModel::setMesh(Mesh *mesh)
{
    if (m_mesh == mesh)
        return;

    beginResetModel();
    m_mesh = mesh;
    emit meshChanged(m_mesh);
    updateModelData();
    endResetModel();
}

void SubsetDataTableModel::setSubsetIndex(int subsetIndex)
{
    if (m_subsetIndex == subsetIndex)
        return;

    beginResetModel();
    m_subsetIndex = subsetIndex;
    emit subsetIndexChanged(m_subsetIndex);
    updateModelData();
    endResetModel();
}

void SubsetDataTableModel::updateModelData()
{
    m_fields.clear();
    if (!m_mesh)
        return;

    const auto &subsets = m_mesh->subsets();
    if (m_subsetIndex > subsets.count())
        return;

    const auto &subset = subsets[m_subsetIndex];
    const int count = subset->count();

    // We need to know which attributes have data for this mesh
    if (subset->positions().size() == count)
        m_fields.append(AttributeField("Position", AttributeField::Attribute::Position));
    const auto &uvKeys = subset->uvs().keys();
    for (int key : uvKeys) {
        if (subset->uvs().value(key).count() == count) {
            QString uvField = QStringLiteral("UV") + QString::number(key);
            m_fields.append(AttributeField(uvField, AttributeField::Attribute::UV, key));
        }
    }
    if (subset->normals().size() == count)
        m_fields.append(AttributeField("Normal", AttributeField::Attribute::Normal));
    if (subset->tangents().size() == count)
        m_fields.append(AttributeField("Tangent", AttributeField::Attribute::Tangent));
    if (subset->binormals().size() == count)
        m_fields.append(AttributeField("Binormal", AttributeField::Attribute::Binormal));
    if (subset->colors().size() == count)
        m_fields.append(AttributeField("Color", AttributeField::Attribute::Color));
    if (subset->joints().size() == count)
        m_fields.append(AttributeField("Joint", AttributeField::Attribute::Joint));
    if (subset->weights().size() == count)
        m_fields.append(AttributeField("Weight", AttributeField::Attribute::Weight));

    const auto &morphTargetPostionKeys = subset->morphTargetPositions().keys();
    for (int key : morphTargetPostionKeys) {
        if (subset->morphTargetPositions().value(key).count() == count) {
            QString morphTargetField = QStringLiteral("MorphPosition") + QString::number(key);
            m_fields.append(AttributeField(morphTargetField, AttributeField::Attribute::MorphTargetPosition, key));
        }
    }

    const auto &morphTargetNormalKeys = subset->morphTargetNormals().keys();
    for (int key : morphTargetNormalKeys) {
        if (subset->morphTargetNormals().value(key).count() == count) {
            QString morphTargetField = QStringLiteral("MorphNormal") + QString::number(key);
            m_fields.append(AttributeField(morphTargetField, AttributeField::Attribute::MorphTargetNormal, key));
        }
    }

    const auto &morphTargetTangentKeys = subset->morphTargetTangents().keys();
    for (int key : morphTargetTangentKeys) {
        if (subset->morphTargetTangents().value(key).count() == count) {
            QString morphTargetField = QStringLiteral("MorphTangent") + QString::number(key);
            m_fields.append(AttributeField(morphTargetField, AttributeField::Attribute::MorphTargetTangent, key));
        }
    }

    const auto &morphTargetBinormalKeys = subset->morphTargetBinormals().keys();
    for (int key : morphTargetBinormalKeys) {
        if (subset->morphTargetBinormals().value(key).count() == count) {
            QString morphTargetField = QStringLiteral("MorphBinormal") + QString::number(key);
            m_fields.append(AttributeField(morphTargetField, AttributeField::Attribute::MorphTargetBinormal, key));
        }
    }

    beginInsertRows(QModelIndex(), 0, count);
    beginInsertColumns(QModelIndex(), 0, m_fields.count() + 1);
    endInsertColumns();
    endInsertRows();

    emit dataChanged(QAbstractItemModel::createIndex(0, 0),
                     QAbstractItemModel::createIndex(count, m_fields.count() + 1));
}

int SubsetDataTableModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !m_mesh)
        return 0;

    const auto &subsets = m_mesh->subsets();
    if (m_subsetIndex > subsets.count())
        return 0;

    const auto &subset = subsets[m_subsetIndex];
    return subset->count();
}

int SubsetDataTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_mesh)
        return 0;

    return m_fields.count() + 1;
}

QVariant SubsetDataTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_mesh || role != Qt::DisplayRole)
        return QVariant();

    const int vertex = index.row();
    if (index.column() == 0) {
        return vertex;
    }

    const auto &attributeField = m_fields[index.column() - 1];

    auto vector2DToString = [](const QVector2D &vector) {
        return QString("(" + QString::number(vector.x()) + ", " +
                       QString::number(vector.y()) + ")");
    };
    auto vector3DToString = [](const QVector3D &vector) {
        return QString("(" + QString::number(vector.x()) + ", " +
                       QString::number(vector.y()) + ", " +
                       QString::number(vector.z()) + ")");
    };
    auto vector4DToString = [](const QVector4D &vector) {
        return QString("(" + QString::number(vector.x()) + ", " +
                       QString::number(vector.y()) + ", " +
                       QString::number(vector.z()) + ", " +
                       QString::number(vector.w()) + ")");
    };

    const auto &subsets = m_mesh->subsets();
    if (m_subsetIndex > subsets.count())
        return 0;

    const auto &subset = subsets[m_subsetIndex];

    if (attributeField.attribute == AttributeField::Attribute::Position) {
        return vector3DToString(subset->positions().at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::Normal) {
        return vector3DToString(subset->normals().at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::Tangent) {
        return vector3DToString(subset->tangents().at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::Binormal) {
        return vector3DToString(subset->binormals().at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::Color) {
        return vector4DToString(subset->colors().at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::Joint) {
        return vector4DToString(subset->joints().at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::Weight) {
        return vector4DToString(subset->weights().at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::UV) {
        const auto &uvList = subset->uvs().value(attributeField.index);
        return vector2DToString(uvList.at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::MorphTargetPosition) {
        const auto &morphTargetList = subset->morphTargetPositions().value(attributeField.index);
        return vector3DToString(morphTargetList.at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::MorphTargetNormal) {
        const auto &morphTargetList = subset->morphTargetNormals().value(attributeField.index);
        return vector3DToString(morphTargetList.at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::MorphTargetTangent) {
        const auto &morphTargetList = subset->morphTargetTangents().value(attributeField.index);
        return vector3DToString(morphTargetList.at(vertex));
    } else if (attributeField.attribute == AttributeField::Attribute::MorphTargetBinormal) {
        const auto &morphTargetList = subset->morphTargetBinormals().value(attributeField.index);
        return vector3DToString(morphTargetList.at(vertex));
    }

    return QVariant();
}

QHash<int, QByteArray> SubsetDataTableModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"} };
}

int SubsetDataTableModel::subsetIndex() const
{
    return m_subsetIndex;
}

QVariant SubsetDataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical) {
        return QString::number(section);
    }

    if (section == 0)
        return QStringLiteral("Vertex");

    const auto &attributeField = m_fields[section - 1];
    return attributeField.headerName;
}

QVector3D SubsetDataTableModel::vertexPositionAtRow(int row)
{
    if (row < 0 || row > rowCount(QModelIndex()))
        return QVector3D();

    const auto &subsets = m_mesh->subsets();
    if (m_subsetIndex > subsets.count())
        return QVector3D();

    const auto &subset = subsets[m_subsetIndex];
    return subset->positions().at(row);
}
