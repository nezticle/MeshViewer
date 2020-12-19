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

#include "mesh.h"
#include <QFile>
#include <QDataStream>


Mesh::Mesh()
{

}

Mesh::~Mesh()
{
    qDeleteAll(m_subsets);
}

quint64 Mesh::loadMesh(const QString &meshFile, quint64 offset)
{
    QFile file(meshFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to load mesh in file: " << meshFile;
        return 0;
    }

    file.seek(offset);
    QDataStream inputStream(&file);
    inputStream.setByteOrder(QDataStream::LittleEndian);
    inputStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    inputStream >> m_meshInfo.fileId >> m_meshInfo.fileVersion >> m_meshInfo.headerFlags >> m_meshInfo.sizeInBytes;

    if (!m_meshInfo.isValid()) {
        file.close();
        qWarning() << "Mesh data invalid";
        return 0;
    }

    MeshOffsetTracker offsetTracker(offset + 12);
    file.seek(offsetTracker.offset());

    // Vertex Buffer
    quint32 vertexBufferEntriesOffset;
    quint32 vertexBufferEntiresSize;
    quint32 vertexBufferDataOffset;
    quint32 vertexBufferDataSize;
    inputStream >> vertexBufferEntriesOffset
                >> vertexBufferEntiresSize
                >> m_vertexBuffer.stride
                >> vertexBufferDataOffset
                >> vertexBufferDataSize;
    // Index Buffer
    quint32 indexBufferComponentType;
    quint32 indexBufferOffset;
    quint32 indexBufferSize;
    inputStream >> indexBufferComponentType
                >> indexBufferOffset
                >> indexBufferSize;
    m_indexBuffer.componentType = ComponentType(indexBufferComponentType);
    // Subsets
    quint32 subsetsOffsets;
    quint32 subsetsSize;
    inputStream >> subsetsOffsets >> subsetsSize;

    // Joints
    quint32 jointsOffsets;
    quint32 jointsSize;
    inputStream >> jointsOffsets >> jointsSize;

    quint32 drawMode;
    quint32 windingMode;
    inputStream >> drawMode >> windingMode;
    m_drawMode = DrawMode(drawMode);
    m_windingMode = WindingMode(windingMode);

    // Mesh structure has been read, advance by the size of that
    offsetTracker.advance(56);

    // Vertex Buffer Entries
    quint32 entriesByteSize = 0;
    for (int i = 0; i < vertexBufferEntiresSize; ++i) {
        VertexBufferEntry vertexBufferEntry;
        quint32 componentType;
        quint32 nameOffset; // ignored
        inputStream >> nameOffset
                    >> componentType
                    >> vertexBufferEntry.numComponents
                    >> vertexBufferEntry.firstItemOffset;
        vertexBufferEntry.componentType = ComponentType(componentType);
        m_vertexBuffer.entires.append(vertexBufferEntry);
        entriesByteSize += 16;
    }
    offsetTracker.alignedAdvance(entriesByteSize);
    file.seek(offsetTracker.offset());
    // Get the names of the vertex entries
    for (auto &entry : m_vertexBuffer.entires) {
        quint32 nameLength;
        inputStream >> nameLength;
        offsetTracker.advance(4);
        entry.name = file.read(nameLength);
        offsetTracker.alignedAdvance(nameLength);
        file.seek(offsetTracker.offset());
    }

    // Vertex Buffer Data
    m_vertexBuffer.data = file.read(vertexBufferDataSize);
    offsetTracker.alignedAdvance(vertexBufferDataSize);
    file.seek(offsetTracker.offset());

    // Index Buffer Data
    m_indexBuffer.data = file.read(indexBufferSize);
    offsetTracker.alignedAdvance(indexBufferSize);
    file.seek(offsetTracker.offset());


    // Subsets
    quint32 subsetByteSize = 0;
    for (int i = 0; i < subsetsSize; ++i) {
        MeshSubset subset;
        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;
        quint32 nameOffset;
        inputStream >> subset.count
                    >> subset.offset
                    >> minX
                    >> minY
                    >> minZ
                    >> maxX
                    >> maxY
                    >> maxZ
                    >> nameOffset
                    >> subset.nameLength;
        subset.bounds.min = QVector3D(minX, minY, minZ);
        subset.bounds.max = QVector3D(maxX, maxY, maxZ);
        subsetByteSize += 40;
        m_meshSubsets.append(subset);
    }
    // Basically forces a 4 byte padding after subsets
    offsetTracker.alignedAdvance(subsetByteSize);
    file.seek(offsetTracker.offset());

    // Subset names
    for (auto &subset : m_meshSubsets) {
        subset.name = file.read(subset.nameLength * 2); //UTF_16_le
        offsetTracker.alignedAdvance(subset.nameLength * 2);
        file.seek(offsetTracker.offset());
    }

    // Joints
    for (int i = 0; i < jointsSize; ++i) {
        Joint joint;
        inputStream >> joint.jointId >> joint.parentId;
        float invBindPos[16];
        for (int j = 0; j < 16; ++j)
            inputStream >> invBindPos[j];
        float localToGlobalBoneSpace[16];
        for (int j = 0; j < 16; ++j)
            inputStream >> localToGlobalBoneSpace[j];
        joint.invBindPos = QMatrix4x4(invBindPos);
        joint.localToGlobalBoneSpace = QMatrix4x4(localToGlobalBoneSpace);
        offsetTracker.alignedAdvance(136);
        file.seek(offsetTracker.offset());
        m_joints.append(joint);
    }

    file.close();

    // Generate Subset Data
    for (int i = 0; i < m_meshSubsets.count(); ++i) {
        auto subset = new Subset(*this, i);
        m_subsets.append(subset);
    }

    return m_meshInfo.sizeInBytes;
}

quint64 Mesh::saveMesh(const QString &meshFile, quint64 offset)
{
    return 0;
}

MeshFileTool::MeshFileTool()
{

}

QVector<Mesh *> MeshFileTool::loadMeshFile(const QString &meshFile)
{
    QFile file(meshFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file: " << meshFile;
        return QVector<Mesh *>();
    }

    QDataStream inputStream(&file);
    inputStream.setByteOrder(QDataStream::LittleEndian);
    file.seek(file.size() - 16);

    MultiMeshInfo meshFileInfo;
    inputStream >> meshFileInfo.fileId >> meshFileInfo.fileVersion;
    file.seek(file.pos() + 4);
    quint32 meshCount;
    inputStream >> meshCount;

    for (int i = 0; i < meshCount; ++i) {
        file.seek(-16 + (-16 * meshCount) + (16 * i) + file.size());
        quint64 offset;
        quint32 id;
        inputStream >> offset >> id;
        meshFileInfo.meshEntires.insert(id, offset);
    }
    file.close();

    if (!meshFileInfo.isValid())
        return QVector<Mesh *>();

    QVector<Mesh *> meshes;

    // Load mesh for each entry
    for (auto key : meshFileInfo.meshEntires.keys()) {
        Mesh *mesh = new Mesh();
        quint64 result = mesh->loadMesh(meshFile, meshFileInfo.meshEntires[key]);
        if (result > 0)
            meshes.append(mesh);
        else
            delete mesh;
    }

    return meshes;
}

void MeshFileTool::saveMeshFile(const QString &meshFile, const QVector<Mesh *> meshes)
{

}

Mesh::Subset::Subset(const Mesh &mesh, int subsetIndex)
{
    // Construct subset information
    static const QByteArray c_positionAttributeName = "attr_pos";
    static const QByteArray c_normalAttributeName = "attr_norm";
    static const QByteArray c_uvAttributeName = "attr_uv"; // + number
    static const QByteArray c_tangentAttributeName = "attr_textan";
    static const QByteArray c_binormalAttributeName = "attr_binormal";
    static const QByteArray c_colorAttributeName = "attr_color";
    static const QByteArray c_jointAttributeName = "attr_joints";
    static const QByteArray c_weightAttributeName = "attr_weights";
    static const QByteArray c_morphTargetAttributeName = "attr_t"; // + number

    const MeshSubset &subset = mesh.m_meshSubsets[subsetIndex];
    m_name = QString::fromUtf16(reinterpret_cast<const char16_t *>(subset.name.data()));
    m_count = subset.count; // not quite true
    m_bounds.min = subset.bounds.min;
    m_bounds.max = subset.bounds.max;
    m_drawMode = mesh.m_drawMode;
    m_windingMode = mesh.m_windingMode;

    // Attributes
    // Index (need full index table to do lookups)
    QVector<quint32> indexes;
    if (mesh.m_indexBuffer.componentType == ComponentType::UnsignedInt16) {
        const quint16 *p = reinterpret_cast<const quint16 *>(mesh.m_indexBuffer.data.data());
        const int length = mesh.m_indexBuffer.data.size() / sizeof(quint16);
        indexes.resize(length);
        for (int i = 0; i < length; ++i)
            indexes[i] = quint32(p[i]);
    } else if (mesh.m_indexBuffer.componentType == ComponentType::UnsignedInt32) {
        const quint32 *p = reinterpret_cast<const quint32 *>(mesh.m_indexBuffer.data.data());
        const int length = mesh.m_indexBuffer.data.size() / sizeof(quint32);
        indexes.resize(length);
        for (int i = 0; i < length; ++i)
            indexes[i] = p[i];
    }

    const quint32 stride = mesh.m_vertexBuffer.stride;

    const quint32 count = subset.count;
    const quint32 offset = subset.offset;

    // Position
    int positionOffset = -1;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_positionAttributeName)) {
            positionOffset = entry.firstItemOffset;
            break;
        }
    }
    if (positionOffset > -1) {
        m_positions.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + positionOffset;
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            m_positions[i] = *p;
        }
    }

    // Normal
    int normalOffset = -1;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_normalAttributeName)) {
            normalOffset = entry.firstItemOffset;
            break;
        }
    }
    if (normalOffset > -1) {
        m_normals.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + normalOffset;
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            m_normals[i] = *p;
        }
    }

    // UV(s)
    QMap<int, int> uvOffsetMap;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_uvAttributeName)) {
            // Need to break down the name to get the UV channel
            QString attributeName = QString::fromLocal8Bit(entry.name);
            QString uvIndexString = attributeName.remove(c_uvAttributeName);
            int uvIndex = uvIndexString.toInt();
            uvOffsetMap.insert(uvIndex, entry.firstItemOffset);
        }
    }
    for (int uvIndex : uvOffsetMap.keys()) {
        QVector<QVector2D> uvData;
        uvData.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + uvOffsetMap[uvIndex];
            const QVector2D *p = reinterpret_cast<const QVector2D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            uvData[i] = *p;
        }
        m_uvs.insert(uvIndex, uvData);
    }

    // Tangents
    int tangentOffset = -1;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_tangentAttributeName)) {
            tangentOffset = entry.firstItemOffset;
            break;
        }
    }
    if (tangentOffset > -1) {
        m_tangents.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + tangentOffset;
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            m_tangents[i] = *p;
        }
    }

    // Binormals
    int binormalsOffset = -1;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_binormalAttributeName)) {
            binormalsOffset = entry.firstItemOffset;
            break;
        }
    }
    if (binormalsOffset > -1) {
        m_binormals.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + binormalsOffset;
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            m_binormals[i] = *p;
        }
    }

    // Colors
    int colorsOffset = -1;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_colorAttributeName)) {
            colorsOffset = entry.firstItemOffset;
            break;
        }
    }
    if (colorsOffset > -1) {
        m_colors.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + colorsOffset;
            const QVector4D *p = reinterpret_cast<const QVector4D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            m_colors[i] = *p;
        }
    }

    // Joints
    int jointsOffset = -1;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_jointAttributeName)) {
            jointsOffset = entry.firstItemOffset;
            break;
        }
    }
    if (jointsOffset > -1) {
        m_joints.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + jointsOffset;
            const QVector4D *p = reinterpret_cast<const QVector4D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            m_joints[i] = *p;
        }
    }

    // Weights
    int weightsOffset = -1;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_weightAttributeName)) {
            weightsOffset = entry.firstItemOffset;
            break;
        }
    }
    if (weightsOffset > -1) {
        m_weights.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + weightsOffset;
            const QVector4D *p = reinterpret_cast<const QVector4D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            m_weights[i] = *p;
        }
    }

    // Morph Target(s)
    QMap<int, int> morphTargetPositionOffsetMap;
    QMap<int, int> morphTargetNormalOffsetMap;
    QMap<int, int> morphTargetTangentOffsetMap;
    QMap<int, int> morphTargetBinormalOffsetMap;
    for (const auto &entry : mesh.m_vertexBuffer.entires) {
        if (entry.name.contains(c_morphTargetAttributeName)) {
            // Need to break down the name to get the UV channel
            QString attributeName = QString::fromLocal8Bit(entry.name);
            // Handle special case
            if (attributeName == QStringLiteral("attr_textan"))
                continue;
            QString morphTargetString = attributeName.remove(c_morphTargetAttributeName);
            if (morphTargetString.contains("pos")) {
                int morphTargetPositionIndex = morphTargetString.remove("pos").toInt();
                morphTargetPositionOffsetMap.insert(morphTargetPositionIndex, entry.firstItemOffset);
            } else if (morphTargetString.contains("norm")) {
                int morphTargetNormalIndex = morphTargetString.remove("norm").toInt();
                morphTargetNormalOffsetMap.insert(morphTargetNormalIndex, entry.firstItemOffset);
            } else if (morphTargetString.contains("tan")) {
                int morphTargetTangentIndex = morphTargetString.remove("tan").toInt();
                morphTargetTangentOffsetMap.insert(morphTargetTangentIndex, entry.firstItemOffset);
            } else if (morphTargetString.contains("binorm")) {
                int morphTargetBinormalIndex = morphTargetString.remove("binorm").toInt();
                morphTargetBinormalOffsetMap.insert(morphTargetBinormalIndex, entry.firstItemOffset);
            }
        }
    }
    // Morph Target Position
    for (int positionIndex : morphTargetPositionOffsetMap.keys()) {
        QVector<QVector3D> posData;
        posData.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + morphTargetPositionOffsetMap[positionIndex];
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            posData[i] = *p;
        }
        m_morphTargetPositions.insert(positionIndex, posData);
    }

    // Morph Target Normal
    for (int normalIndex : morphTargetNormalOffsetMap.keys()) {
        QVector<QVector3D> normalData;
        normalData.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + morphTargetNormalOffsetMap[normalIndex];
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            normalData[i] = *p;
        }
        m_morphTargetNormals.insert(normalIndex, normalData);
    }

    // Morph Target Tangent
    for (int tangentIndex : morphTargetTangentOffsetMap.keys()) {
        QVector<QVector3D> tangentData;
        tangentData.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + morphTargetTangentOffsetMap[tangentIndex];
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            tangentData[i] = *p;
        }
        m_morphTargetTangents.insert(tangentIndex, tangentData);
    }

    // Morph Target Tangent
    for (int binormalIndex : morphTargetBinormalOffsetMap.keys()) {
        QVector<QVector3D> binormalData;
        binormalData.resize(count);
        for (int i = 0; i < count; ++i) {
            const quint32 localByteOffset = stride * indexes[i + offset] + morphTargetBinormalOffsetMap[binormalIndex];
            const QVector3D *p = reinterpret_cast<const QVector3D *>(mesh.m_vertexBuffer.data.data() + localByteOffset);
            binormalData[i] = *p;
        }
        m_morphTargetBinormals.insert(binormalIndex, binormalData);
    }
}

QMap<int, QVector<QVector3D> > Mesh::Subset::morphTargetBinormals() const
{
    return m_morphTargetBinormals;
}

Mesh::DrawMode Mesh::Subset::drawMode() const
{
    return m_drawMode;
}

int Mesh::Subset::count() const
{
    return m_count;
}

Mesh::WindingMode Mesh::Subset::windingMode() const
{
    return m_windingMode;
}

Mesh::MeshSubsetBounds Mesh::Subset::bounds() const
{
    return m_bounds;
}

QString Mesh::Subset::name() const
{
    return m_name;
}

QMap<int, QVector<QVector3D> > Mesh::Subset::morphTargetTangents() const
{
    return m_morphTargetTangents;
}

QMap<int, QVector<QVector3D> > Mesh::Subset::morphTargetNormals() const
{
    return m_morphTargetNormals;
}

QMap<int, QVector<QVector3D> > Mesh::Subset::morphTargetPositions() const
{
    return m_morphTargetPositions;
}

QVector<QVector4D> Mesh::Subset::weights() const
{
    return m_weights;
}

QVector<QVector4D> Mesh::Subset::joints() const
{
    return m_joints;
}

QVector<QVector4D> Mesh::Subset::colors() const
{
    return m_colors;
}

QVector<QVector3D> Mesh::Subset::binormals() const
{
    return m_binormals;
}

QVector<QVector3D> Mesh::Subset::tangents() const
{
    return m_tangents;
}

QMap<int, QVector<QVector2D> > Mesh::Subset::uvs() const
{
    return m_uvs;
}

QVector<QVector3D> Mesh::Subset::normals() const
{
    return m_normals;
}

QVector<QVector3D> Mesh::Subset::positions() const
{
    return m_positions;
}
