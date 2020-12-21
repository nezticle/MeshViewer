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
    Q_PROPERTY(QQuick3DGeometry* wireframe READ wireframe NOTIFY wireframeChanged)
    Q_PROPERTY(MeshInfo* meshInfo READ meshInfo WRITE setMeshInfo NOTIFY meshInfoChanged)
    Q_PROPERTY(int subsetIndex READ subsetIndex WRITE setSubsetIndex NOTIFY subsetIndexChanged)
    QML_ELEMENT
public:
    GeometryGenerator(QQuick3DObject *parent = nullptr);
    void setSubset(Mesh::Subset *subset);

    QQuick3DGeometry* original() const;
    QQuick3DGeometry* wireframe() const;
    MeshInfo* meshInfo() const;
    int subsetIndex() const;

public slots:
    void setMeshInfo(MeshInfo* meshInfo);
    void setSubsetIndex(int subsetIndex);

private slots:
    void updateSubset();

signals:
    void originalChanged(QQuick3DGeometry* original);
    void wireframeChanged(QQuick3DGeometry* wireframe);
    void meshInfoChanged(MeshInfo* meshInfo);
    void subsetIndexChanged(int subsetIndex);

private:
    void generate();
    void generateOriginalGeometry();
    void generateWireframeGeometry();

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
