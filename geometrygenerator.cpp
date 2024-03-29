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

#include "geometrygenerator.h"

GeometryGenerator::GeometryGenerator(QQuick3DObject *parent)
    : QQuick3DObject(parent)
{

}

void GeometryGenerator::setSubset(Mesh::Subset *subset)
{
    if (subset == m_subset)
        return;

    m_subset = subset;
    generate();
}

QQuick3DGeometry *GeometryGenerator::original() const
{
    return m_originalGeometry;
}

QQuick3DGeometry *GeometryGenerator::wireframe() const
{
    return m_wireframeGeometry;
}

QQuick3DGeometry *GeometryGenerator::normals() const
{
    return m_normalsLinesGeometry;
}

QQuick3DGeometry *GeometryGenerator::tangents() const
{
    return m_tangetsLinesGeometry;
}

QQuick3DGeometry *GeometryGenerator::binormals() const
{
    return m_binormalsLinesGeometry;
}

MeshInfo *GeometryGenerator::meshInfo() const
{
    return m_meshInfo;
}

int GeometryGenerator::subsetIndex() const
{
    return m_subsetIndex;
}

float GeometryGenerator::scaleFactor() const
{
    return m_scaleFactor;
}

void GeometryGenerator::setMeshInfo(MeshInfo *meshInfo)
{
    if (m_meshInfo == meshInfo)
        return;

    if (m_meshInfo)
        disconnect(m_meshInfo, &MeshInfo::meshesUpdated, this, &GeometryGenerator::updateSubset);

    m_meshInfo = meshInfo;
    emit meshInfoChanged(m_meshInfo);
    updateSubset();
    if (m_meshInfo)
        connect(m_meshInfo, &MeshInfo::meshesUpdated, this, &GeometryGenerator::updateSubset);
}

void GeometryGenerator::setSubsetIndex(int subsetIndex)
{
    if (m_subsetIndex == subsetIndex)
        return;

    m_subsetIndex = subsetIndex;
    emit subsetIndexChanged(m_subsetIndex);
    updateSubset();
}

void GeometryGenerator::updateSubset()
{
    Mesh::Subset *subset = nullptr;

    if (m_meshInfo) {
        auto mesh = m_meshInfo->mesh();
        if (mesh) {
            const auto &subsets = mesh->subsets();
            if (m_subsetIndex < subsets.count()) {
                subset = subsets.at(m_subsetIndex);
            }
        }
    }

    if (subset == m_subset)
        return;

    m_subset = subset;
    // Update Scale Factor
    // Scale factor is 1/100 of the largest bounds extents
    const QVector3D extents = m_subset->bounds().max - m_subset->bounds().min;
    float maxExtent = qMax(qMax(extents.x(), extents.y()), extents.z());
    m_scaleFactor = maxExtent / 100.0f;
    emit scaleFactorChanged(m_scaleFactor);
    generate();
}

void GeometryGenerator::generate()
{
    // Cleanup any old geometry
    if (m_originalGeometry)
        delete m_originalGeometry;
    if (m_wireframeGeometry)
        delete m_wireframeGeometry;
    if (m_normalsLinesGeometry)
        delete m_normalsLinesGeometry;
    if (m_tangetsLinesGeometry)
        delete m_tangetsLinesGeometry;
    if (m_binormalsLinesGeometry)
        delete m_binormalsLinesGeometry;

    if (!m_subset)
        return;

    if (m_subset->drawMode() != Mesh::DrawMode::Triangles)
        return;

    generateOriginalGeometry();
    generateWireframeGeometry();
    generateNormalGeometry();
    generateTangentGeometry();
    generateBinormalGeometry();
}

void GeometryGenerator::generateOriginalGeometry()
{
    m_originalGeometry = new QQuick3DGeometry(this);

    const auto &positions = m_subset->positions();
    const auto &normals = m_subset->normals();
    const auto &uvs = m_subset->uvs();
    const auto &tangents = m_subset->tangents();
    const auto &binormals = m_subset->binormals();
    const auto &colors = m_subset->colors();
    const int count = m_subset->count();

    // Calculate stride
    quint32 stride = 0;
    if (positions.count() == count) {
        m_originalGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                                         stride,
                                         QQuick3DGeometry::Attribute::F32Type);
        stride += sizeof(QVector3D);
    }
    if (normals.count() == count) {
        m_originalGeometry->addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                                         stride,
                                         QQuick3DGeometry::Attribute::F32Type);
        stride += sizeof(QVector3D);
    }
    const auto &keys = uvs.keys();
    if (keys.count() > 0) {
        for (auto key : keys) {
            // Only care about UV0 for now... (bugs)
            if (key != 0)
                continue;
            const auto &uv = uvs[key];
            if (uv.count() == count) {
                m_originalGeometry->addAttribute(QQuick3DGeometry::Attribute::TexCoordSemantic,
                                                 stride,
                                                 QQuick3DGeometry::Attribute::F32Type);
                stride += sizeof(QVector2D);
            }
        }
    }
    if (tangents.count() == count) {
        m_originalGeometry->addAttribute(QQuick3DGeometry::Attribute::TangentSemantic,
                                         stride,
                                         QQuick3DGeometry::Attribute::F32Type);
        stride += sizeof(QVector3D);
    }
    if (binormals.count() == count) {
        m_originalGeometry->addAttribute(QQuick3DGeometry::Attribute::BinormalSemantic,
                                         stride,
                                         QQuick3DGeometry::Attribute::F32Type);
        stride += sizeof(QVector3D);
    }
    if (colors.count() == count) {
        m_originalGeometry->addAttribute(QQuick3DGeometry::Attribute::ColorSemantic,
                                         stride,
                                         QQuick3DGeometry::Attribute::F32Type);
        stride += sizeof(QVector4D);
    }

    m_originalGeometry->setStride(stride);
    m_originalGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
    m_originalGeometry->setBounds(m_subset->bounds().min, m_subset->bounds().max);

    QByteArray vertexBuffer;
    vertexBuffer.resize(stride * count);

    float *p = reinterpret_cast<float *>(vertexBuffer.data());

    for (int i = 0; i < count; ++i) {
        if (positions.count() == count) {
            const auto &pos = positions.at(i);
            *p++ = pos.x();
            *p++ = pos.y();
            *p++ = pos.z();
        }
        if (normals.count() == count) {
            const auto &normal = normals.at(i);
            *p++ = normal.x();
            *p++ = normal.y();
            *p++ = normal.z();
        }
        const auto &keys = uvs.keys();
        if (keys.count() > 0) {
            for (auto key : keys) {
                // Only care about UV0 for now... (bugs)
                if (key != 0)
                    continue;
                const auto &uv = uvs[key];
                if (uv.count() == count) {
                    const auto &coord = uv.at(i);
                    *p++ = coord.x();
                    *p++ = coord.y();
                }
            }
        }
        if (tangents.count() == count) {
            const auto &tangent = tangents.at(i);
            *p++ = tangent.x();
            *p++ = tangent.y();
            *p++ = tangent.z();
        }
        if (binormals.count() == count) {
            const auto &binormal = binormals.at(i);
            *p++ = binormal.x();
            *p++ = binormal.y();
            *p++ = binormal.z();
        }
        if (colors.count() == count) {
            const auto &color = colors.at(i);
            *p++ = color.x();
            *p++ = color.y();
            *p++ = color.z();
            *p++ = color.w();
        }
    }
    m_originalGeometry->setVertexData(vertexBuffer);
    emit originalChanged(m_originalGeometry);
}

void GeometryGenerator::generateWireframeGeometry()
{
    m_wireframeGeometry = new QQuick3DGeometry(this);

    const auto &positions = m_subset->positions();
    const auto &normals = m_subset->normals();
    const int count = m_subset->count();

    const quint32 stride = 6 * sizeof(float);
    const bool hasNormals = normals.count() == count;

    m_wireframeGeometry->setStride(stride);
    m_wireframeGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
    m_wireframeGeometry->setBounds(m_subset->bounds().min, m_subset->bounds().max);

    m_wireframeGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                                      0,
                                      QQuick3DGeometry::Attribute::F32Type);
    m_wireframeGeometry->addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                                      3 * sizeof(float),
                                      QQuick3DGeometry::Attribute::F32Type);
    m_wireframeGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                                      0,
                                      QQuick3DGeometry::Attribute::U32Type);

    QByteArray vertexBuffer;
    vertexBuffer.resize(count * stride);
    float *vp = reinterpret_cast<float *>(vertexBuffer.data());

    QByteArray indexBuffer;
    indexBuffer.resize(count * 2 * sizeof(quint32));
    quint32 *ip = reinterpret_cast<quint32 *>(indexBuffer.data());

    for (int i = 0; i < count; i += 3) {
        const QVector3D &pos1 = positions.at(i);
        const QVector3D &pos2 = positions.at(i+1);
        const QVector3D &pos3 = positions.at(i+2);
        QVector3D normal;
        if (hasNormals) {
            normal = normals.at(i) + normals.at(i+1) + normals.at(i+2);
            normal.normalize();
        } else {
            const QVector3D v = pos2 - pos1;
            const QVector3D w = pos3 - pos1;
            normal = QVector3D::crossProduct(v, w).normalized();
        }
        // Vertex Buffer
        // 0
        *vp++ = pos1.x();
        *vp++ = pos1.y();
        *vp++ = pos1.z();
        *vp++ = normal.x();
        *vp++ = normal.y();
        *vp++ = normal.z();

        // 1
        *vp++ = pos2.x();
        *vp++ = pos2.y();
        *vp++ = pos2.z();
        *vp++ = normal.x();
        *vp++ = normal.y();
        *vp++ = normal.z();

        // 2
        *vp++ = pos3.x();
        *vp++ = pos3.y();
        *vp++ = pos3.z();
        *vp++ = normal.x();
        *vp++ = normal.y();
        *vp++ = normal.z();

        // Index Buffer
        *ip++ = i;
        *ip++ = i+1;

        *ip++ = i+1;
        *ip++ = i+2;

        *ip++ = i+2;
        *ip++ = i;
    }
    m_wireframeGeometry->setVertexData(vertexBuffer);
    m_wireframeGeometry->setIndexData(indexBuffer);
    emit wireframeChanged(m_wireframeGeometry);
}

void GeometryGenerator::generateNormalGeometry()
{
    m_normalsLinesGeometry = new QQuick3DGeometry(this);
    const auto &positions = m_subset->positions();
    const auto &normals = m_subset->normals();
    const int count = m_subset->count();

    const quint32 stride = 3 * sizeof(float);
    const bool hasNormals = normals.count() == count;

    m_normalsLinesGeometry->setStride(stride);
    m_normalsLinesGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
    // TODO: Not quite true
    m_normalsLinesGeometry->setBounds(m_subset->bounds().min, m_subset->bounds().max);

    m_normalsLinesGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                                         0,
                                         QQuick3DGeometry::Attribute::F32Type);
    m_normalsLinesGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                                         0,
                                         QQuick3DGeometry::Attribute::U32Type);

    // If there are no normals, just do face normals
    int normalCount = (count / 3);
    // If there are normals, handle each vertex normals as well
    if (hasNormals)
        normalCount += count;

    QByteArray vertexBuffer;
    vertexBuffer.resize(normalCount * 2 * stride);
    QVector3D *vp = reinterpret_cast<QVector3D *>(vertexBuffer.data());

    QByteArray indexBuffer;
    indexBuffer.resize(normalCount * 2 * sizeof(quint32));
    quint32 *ip = reinterpret_cast<quint32 *>(indexBuffer.data());

    quint32 index = 0;
    for (int i = 0; i < count; i += 3) {
        const QVector3D &pos1 = positions.at(i);
        const QVector3D &pos2 = positions.at(i+1);
        const QVector3D &pos3 = positions.at(i+2);
        const QVector3D v = pos2 - pos1;
        const QVector3D w = pos3 - pos1;
        const QVector3D faceNormal = QVector3D::crossProduct(v, w).normalized();
        const QVector3D faceCenter = (pos1 + pos2 + pos3) / 3;
        *vp++ = faceCenter;
        *vp++ = faceCenter + faceNormal * m_scaleFactor;
        *ip++ = index;
        *ip++ = index + 1;
        index += 2;

        if (hasNormals) {
            const QVector3D normal1 = normals.at(i);
            const QVector3D normal2 = normals.at(i+1);
            const QVector3D normal3 = normals.at(i+2);

            *vp++ = pos1;
            *vp++ = pos1 + normal1 * m_scaleFactor;
            *ip++ = index;
            *ip++ = index + 1;

            *vp++ = pos2;
            *vp++ = pos2 + normal2 * m_scaleFactor;
            *ip++ = index + 2;
            *ip++ = index + 3;

            *vp++ = pos3;
            *vp++ = pos3 + normal3 * m_scaleFactor;
            *ip++ = index + 4;
            *ip++ = index + 5;
            index += 6;
        }
    }

    m_normalsLinesGeometry->setVertexData(vertexBuffer);
    m_normalsLinesGeometry->setIndexData(indexBuffer);

    emit normalsChanged(m_normalsLinesGeometry);
}

void GeometryGenerator::generateTangentGeometry()
{
    const auto &positions = m_subset->positions();
    const auto &tangents = m_subset->tangents();
    const int count = m_subset->count();

    const quint32 stride = 3 * sizeof(float);
    const bool hasTangents = tangents.count() == count;

    if (!hasTangents)
        return;

    m_tangetsLinesGeometry = new QQuick3DGeometry(this);

    m_tangetsLinesGeometry->setStride(stride);
    m_tangetsLinesGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
    // TODO: Not quite true
    m_tangetsLinesGeometry->setBounds(m_subset->bounds().min, m_subset->bounds().max);

    m_tangetsLinesGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                                         0,
                                         QQuick3DGeometry::Attribute::F32Type);
    m_tangetsLinesGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                                         0,
                                         QQuick3DGeometry::Attribute::U32Type);

    QByteArray vertexBuffer;
    vertexBuffer.resize(count * 2 * stride);
    QVector3D *vp = reinterpret_cast<QVector3D *>(vertexBuffer.data());

    QByteArray indexBuffer;
    indexBuffer.resize(count * 2 * sizeof(quint32));
    quint32 *ip = reinterpret_cast<quint32 *>(indexBuffer.data());

    quint32 index = 0;
    for (int i = 0; i < count; i += 3) {
        const QVector3D &pos1 = positions.at(i);
        const QVector3D &pos2 = positions.at(i+1);
        const QVector3D &pos3 = positions.at(i+2);

        const QVector3D tangent1 = tangents.at(i);
        const QVector3D tangent2 = tangents.at(i+1);
        const QVector3D tangent3 = tangents.at(i+2);

        *vp++ = pos1;
        *vp++ = pos1 + tangent1 * m_scaleFactor;
        *ip++ = index;
        *ip++ = index + 1;

        *vp++ = pos2;
        *vp++ = pos2 + tangent2 * m_scaleFactor;
        *ip++ = index + 2;
        *ip++ = index + 3;

        *vp++ = pos3;
        *vp++ = pos3 + tangent3 * m_scaleFactor;
        *ip++ = index + 4;
        *ip++ = index + 5;
        index += 6;
    }

    m_tangetsLinesGeometry->setVertexData(vertexBuffer);
    m_tangetsLinesGeometry->setIndexData(indexBuffer);

    emit tangentsChanged(m_tangetsLinesGeometry);
}

void GeometryGenerator::generateBinormalGeometry()
{
    const auto &positions = m_subset->positions();
    const auto &binormals = m_subset->binormals();
    const int count = m_subset->count();

    const quint32 stride = 3 * sizeof(float);
    const bool hasBinormals = binormals.count() == count;

    if (!hasBinormals)
        return;

    m_binormalsLinesGeometry = new QQuick3DGeometry(this);

    m_binormalsLinesGeometry->setStride(stride);
    m_binormalsLinesGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
    // TODO: Not quite true
    m_binormalsLinesGeometry->setBounds(m_subset->bounds().min, m_subset->bounds().max);

    m_binormalsLinesGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                                           0,
                                           QQuick3DGeometry::Attribute::F32Type);
    m_binormalsLinesGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                                           0,
                                           QQuick3DGeometry::Attribute::U32Type);

    QByteArray vertexBuffer;
    vertexBuffer.resize(count * 2 * stride);
    QVector3D *vp = reinterpret_cast<QVector3D *>(vertexBuffer.data());

    QByteArray indexBuffer;
    indexBuffer.resize(count * 2 * sizeof(quint32));
    quint32 *ip = reinterpret_cast<quint32 *>(indexBuffer.data());

    quint32 index = 0;
    for (int i = 0; i < count; i += 3) {
        const QVector3D &pos1 = positions.at(i);
        const QVector3D &pos2 = positions.at(i+1);
        const QVector3D &pos3 = positions.at(i+2);

        const QVector3D binormal1 = binormals.at(i);
        const QVector3D binormal2 = binormals.at(i+1);
        const QVector3D binormal3 = binormals.at(i+2);

        *vp++ = pos1;
        *vp++ = pos1 + binormal1 * m_scaleFactor;
        *ip++ = index;
        *ip++ = index + 1;

        *vp++ = pos2;
        *vp++ = pos2 + binormal2 * m_scaleFactor;
        *ip++ = index + 2;
        *ip++ = index + 3;

        *vp++ = pos3;
        *vp++ = pos3 + binormal3 * m_scaleFactor;
        *ip++ = index + 4;
        *ip++ = index + 5;
        index += 6;
    }

    m_binormalsLinesGeometry->setVertexData(vertexBuffer);
    m_binormalsLinesGeometry->setIndexData(indexBuffer);

    emit binormalsChanged(m_binormalsLinesGeometry);
}

QSSGRenderGraphObject *GeometryGenerator::updateSpatialNode(QSSGRenderGraphObject *node)
{
    return nullptr;
}
