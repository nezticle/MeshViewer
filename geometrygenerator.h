#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include <QtQuick3D/QQuick3DObject>
#include <QtQuick3D/QQuick3DGeometry>

#include "mesh.h"
#include "meshinfo.h"


class GeometryGenerator : public QQuick3DObject
{
    Q_OBJECT
    Q_PROPERTY(QQuick3DGeometry* original READ original NOTIFY originalChanged)
    Q_PROPERTY(MeshInfo* meshInfo READ meshInfo WRITE setMeshInfo NOTIFY meshInfoChanged)
    Q_PROPERTY(int subsetIndex READ subsetIndex WRITE setSubsetIndex NOTIFY subsetIndexChanged)
    QML_ELEMENT
public:
    GeometryGenerator(QQuick3DObject *parent = nullptr);
    void setSubset(Mesh::Subset *subset);

    QQuick3DGeometry* original() const;
    MeshInfo* meshInfo() const;
    int subsetIndex() const;

public slots:
    void setMeshInfo(MeshInfo* meshInfo);
    void setSubsetIndex(int subsetIndex);

private slots:
    void updateSubset();

signals:
    void originalChanged(QQuick3DGeometry* original);
    void meshInfoChanged(MeshInfo* meshInfo);
    void subsetIndexChanged(int subsetIndex);

private:
    void generate();
    void generateOriginalGeometry();

    QQuick3DGeometry *m_originalGeometry = nullptr;
    QQuick3DGeometry *m_wireframeGeometry = nullptr;
    QQuick3DGeometry *m_normalsLinesGeometry = nullptr;
    QQuick3DGeometry *m_tangetsLinesGeometry = nullptr;
    QQuick3DGeometry *m_binormalsLinesGeometry = nullptr;

    Mesh::Subset *m_subset = nullptr;
    MeshInfo* m_meshInfo = nullptr;
    int m_subsetIndex = 0;

    // QQuick3DObject interface
protected:
    QSSGRenderGraphObject *updateSpatialNode(QSSGRenderGraphObject *node) override;
};

#endif // GEOMETRYGENERATOR_H
