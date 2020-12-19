#ifndef MESH_H
#define MESH_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QVector>
#include <QMap>

class Mesh;

class MeshFileTool {
public:
    MeshFileTool();

    QVector<Mesh *> loadMeshFile(const QString &meshFile);
    void saveMeshFile(const QString &meshFile, const QVector<Mesh *> meshes);

private:
    struct MultiMeshInfo
    {
        quint32 fileId = 0;
        quint32 fileVersion = 0;
        QMap<quint32, quint64> meshEntires;

        bool isValid() {
            return fileId == 555777497 && fileVersion == 1;
        }
    };
};

class Mesh
{
public:

    struct MeshSubsetBounds {
        QVector3D min;
        QVector3D max;
    };

    enum DrawMode {
        Points = 1,
        LineStrip,
        LineLoop,
        Lines,
        TriangleStrip,
        TriangleFan,
        Triangles,
        Patches
    };

    enum WindingMode {
        Clockwise = 1,
        CounterClockwise
    };

    class Subset {
    public:
        Subset(const Mesh &mesh, int subsetIndex);

        QVector<QVector3D> positions() const;
        QVector<QVector3D> normals() const;
        QMap<int, QVector<QVector2D> > uvs() const;
        QVector<QVector3D> tangents() const;
        QVector<QVector3D> binormals() const;
        QVector<QVector4D> colors() const;
        QVector<QVector4D> joints() const;
        QVector<QVector4D> weights() const;
        QMap<int, QVector<QVector3D> > morphTargetPositions() const;
        QMap<int, QVector<QVector3D> > morphTargetNormals() const;
        QMap<int, QVector<QVector3D> > morphTargetTangents() const;
        QMap<int, QVector<QVector3D> > morphTargetBinormals() const;

        QString name() const;
        MeshSubsetBounds bounds() const;
        WindingMode windingMode() const;
        DrawMode drawMode() const;
        int count() const;

    private:
        QString m_name;
        MeshSubsetBounds m_bounds;
        WindingMode m_windingMode;
        DrawMode m_drawMode;
        int m_count;
        // Attributes
        QVector<QVector3D> m_positions;
        QVector<QVector3D> m_normals;
        QMap<int, QVector<QVector2D>> m_uvs;
        QVector<QVector3D> m_tangents;
        QVector<QVector3D> m_binormals;
        QVector<QVector4D> m_colors;
        QVector<QVector4D> m_joints;
        QVector<QVector4D> m_weights;
        QMap<int, QVector<QVector3D>> m_morphTargetPositions;
        QMap<int, QVector<QVector3D>> m_morphTargetNormals;
        QMap<int, QVector<QVector3D>> m_morphTargetTangents;
        QMap<int, QVector<QVector3D>> m_morphTargetBinormals;
    };

    Mesh();
    ~Mesh();

    QVector<Subset *> subsets() const { return m_subsets; }

    quint64 loadMesh(const QString &meshFile, quint64 offset);
    quint64 saveMesh(const QString &meshFile, quint64 offset);

private:
    struct MeshDataHeader
    {
        quint32 fileId = 0;
        quint16 fileVersion = 0;
        quint16 headerFlags = 0;
        quint32 sizeInBytes = 0;

        bool isValid() {
            return fileId == 3365961549 && fileVersion == 3;
        }
    };

    struct MeshOffsetTracker
    {
        quint32 startOffset = 0;
        quint32 byteCounter = 0;
        MeshOffsetTracker(int offset)
            : startOffset(offset) {}

        int offset() {
            return startOffset + byteCounter;
        }

        void alignedAdvance(int advanceAmount) {
            advance(advanceAmount);
            quint32 alignmentAmount = 4 - (byteCounter % 4);
            byteCounter += alignmentAmount;
        }

        void advance(int advanceAmount) {
            byteCounter += advanceAmount;
        }
    };

    enum ComponentType {
        UnsignedInt8 = 1,
        Int8,
        UnsignedInt16,
        Int16,
        UnsignedInt32,
        Int32,
        UnsignedInt64,
        Int64,
        Float16,
        Float32,
        Float64
    };

    struct VertexBufferEntry {
        ComponentType componentType = ComponentType::Float32;
        quint32 numComponents = 0;
        quint32 firstItemOffset = 0;
        QByteArray name;
    };

    struct VertexBuffer {
        quint32 stride = 0;
        QVector<VertexBufferEntry> entires;
        QByteArray data;
    };

    struct IndexBuffer {
        ComponentType componentType = ComponentType::UnsignedInt32;
        QByteArray data;
    };

    struct MeshSubset {
        struct MeshSubsetBounds {
            QVector3D min;
            QVector3D max;
        };
        quint32 count = 0;
        quint32 offset = 0;
        MeshSubsetBounds bounds;
        QByteArray name;
        quint32 nameLength = 0;
    };

    struct Joint {
        quint32 jointId = 0;
        quint32 parentId = 0;
        QMatrix4x4 invBindPos;
        QMatrix4x4 localToGlobalBoneSpace;
    };

    MeshDataHeader m_meshInfo;
    VertexBuffer m_vertexBuffer;
    IndexBuffer m_indexBuffer;
    QVector<MeshSubset> m_meshSubsets;
    QVector<Joint> m_joints;
    DrawMode m_drawMode;
    WindingMode m_windingMode;

    // Easy to consume info:
    QVector<Subset *> m_subsets;
};

#endif // MESH_H
