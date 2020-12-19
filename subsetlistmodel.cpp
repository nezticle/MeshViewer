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

#include "subsetlistmodel.h"

SubsetListModel::SubsetListModel()
{

}

Mesh *SubsetListModel::mesh() const
{
    return m_mesh;
}

void SubsetListModel::setMesh(Mesh *mesh)
{
    if (m_mesh == mesh)
        return;

    beginResetModel();
    m_mesh = mesh;
    emit meshChanged(m_mesh);
    endResetModel();
}

int SubsetListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !m_mesh)
        return 0;

    return m_mesh->subsets().count();
}


QVariant SubsetListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_mesh)
        return QVariant();

    const auto &subsets = m_mesh->subsets();
    const int subsetIndex = index.row();

    if (subsetIndex < subsets.count()) {
        if (role == SubsetName) {
            return subsets[subsetIndex]->name();
        }
    }

    return QVariant();
}

QHash<int, QByteArray> SubsetListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[SubsetName] = "subsetName";
    return roles;
}
