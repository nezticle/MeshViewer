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

MeshInfo *GeometryGenerator::meshInfo() const
{
    return m_meshInfo;
}

int GeometryGenerator::subsetIndex() const
{
    return m_subsetIndex;
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
            const auto &normal = positions.at(i);
            *p++ = normal.x();
            *p++ = normal.y();
            *p++ = normal.z();
        }
        const auto &keys = uvs.keys();
        if (keys.count() > 0) {
            for (auto key : keys) {
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


QSSGRenderGraphObject *GeometryGenerator::updateSpatialNode(QSSGRenderGraphObject *node)
{
    return nullptr;
}
