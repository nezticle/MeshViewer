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

#ifndef MESHINFO_H
#define MESHINFO_H

#include <QObject>
#include <qqml.h>

#include "subsetlistmodel.h"
#include "subsetdatatablemodel.h"
#include "mesh.h"

class MeshInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl meshFile READ meshFile WRITE setMeshFile NOTIFY meshFileChanged)
    Q_PROPERTY(SubsetListModel* subsetListModel READ subsetListModel NOTIFY subsetListModelChanged)
    Q_PROPERTY(SubsetDataTableModel* subsetDataTableModel READ subsetDataTableModel NOTIFY subsetDataTableModelChanged)
    QML_ELEMENT
public:
    explicit MeshInfo(QObject *parent = nullptr);
    ~MeshInfo();

    QUrl meshFile() const;
    SubsetListModel* subsetListModel() const;
    SubsetDataTableModel* subsetDataTableModel() const;

public slots:
    void setMeshFile(QUrl meshFile);

signals:
    void meshFileChanged(QUrl meshFile);
    void subsetListModelChanged(SubsetListModel* subsetListModel);
    void subsetDataTableModelChanged(SubsetDataTableModel* subsetDataTableModel);

private:
    void updateSourceMeshFile();
    QUrl m_meshFile;
    SubsetListModel* m_subsetListModel = nullptr;
    SubsetDataTableModel* m_subsetDataTableModel = nullptr;
    QVector<Mesh *> m_meshes;
    MeshFileTool m_meshFileTool;
};

#endif // MESHINFO_H
