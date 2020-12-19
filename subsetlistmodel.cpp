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
