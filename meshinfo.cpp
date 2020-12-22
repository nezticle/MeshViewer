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

#include "meshinfo.h"

#include <QtQml/QQmlFile>
#include <QtQml/QQmlContext>
#include <QFileInfo>

MeshInfo::MeshInfo(QObject *parent) : QObject(parent)
{
    m_subsetListModel = new SubsetListModel();
    m_subsetDataTableModel = new SubsetDataTableModel();
}

MeshInfo::~MeshInfo()
{
    delete m_subsetListModel;
    delete m_subsetDataTableModel;
    if (!m_meshes.isEmpty())
        qDeleteAll(m_meshes);
}

QUrl MeshInfo::meshFile() const
{
    return m_meshFile;
}

SubsetListModel *MeshInfo::subsetListModel() const
{
    return m_subsetListModel;
}

SubsetDataTableModel *MeshInfo::subsetDataTableModel() const
{
    return m_subsetDataTableModel;
}

Mesh *MeshInfo::mesh() const
{
    if (m_meshes.isEmpty())
        return nullptr;

    return m_meshes.first();
}

QString MeshInfo::meshName() const
{
    return m_meshName;
}

void MeshInfo::setMeshFile(QUrl meshFile)
{
    if (m_meshFile == meshFile)
        return;

    m_meshFile = meshFile;
    emit meshFileChanged(m_meshFile);
    updateSourceMeshFile();
}

void MeshInfo::updateSourceMeshFile()
{
    const QQmlContext *context = qmlContext(this);
    const QString meshPath = QQmlFile::urlToLocalFileOrQrc(context ? context->resolvedUrl(m_meshFile) : m_meshFile);
    QFileInfo fileInfo(meshPath);
    m_meshName = fileInfo.fileName();
    emit meshNameChanged(m_meshName);

    // Cleanup
    m_subsetListModel->setMesh(nullptr);
    m_subsetDataTableModel->setMesh(nullptr);

    if (!m_meshes.isEmpty())
        qDeleteAll(m_meshes);

    m_meshes = m_meshFileTool.loadMeshFile(meshPath);

    if (!m_meshes.isEmpty()) {
        m_subsetListModel->setMesh(m_meshes.first());
        m_subsetDataTableModel->setMesh(m_meshes.first());
    }
    emit meshesUpdated();
}
