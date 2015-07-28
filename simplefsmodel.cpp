#include <QFileInfo>
#include <QDir>
#include <algorithm>
#include <QFileIconProvider>
#include <QDateTime>
#include <QDebug>

#include "simplefsmodel.h"

SimpleFSModel::SimpleFSModel(QObject* parent) :
    QAbstractItemModel(parent),
    _metaProvider(new QFileIconProvider())
{
    fetchRootDirectory();
}

SimpleFSModel::~SimpleFSModel()
{}


struct SimpleFSModel::NodeInfo
{
    NodeInfo():
        parent(0),
        mapped(false)
    {}

    NodeInfo(const QFileInfo& fileInfo, NodeInfo* parent = 0):
        fileInfo(fileInfo),
        parent(parent),
        mapped(false)
    {}

    bool operator ==(const NodeInfo& another) const
    {
        bool r = this->fileInfo == another.fileInfo;
        Q_ASSERT(!r || this->parent == another.parent);
        Q_ASSERT(!r || this->mapped == another.mapped);
        Q_ASSERT(!r || this->children == another.children);
        return r;
    }

    QFileInfo fileInfo;
    QVector<NodeInfo> children;
    NodeInfo* parent;

    bool mapped;
};

QModelIndex SimpleFSModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if (!parent.isValid()) { // запрашивают индексы корневых узлов
        return createIndex(row, column, const_cast<NodeInfo*>(&_nodes[row]));
    }

    NodeInfo* parentInfo = static_cast<NodeInfo*>(parent.internalPointer());
    return createIndex(row, column, &parentInfo->children[row]);
}

QModelIndex SimpleFSModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    NodeInfo* childInfo = static_cast<NodeInfo*>(child.internalPointer());
    NodeInfo* parentInfo = childInfo->parent;
    if (parentInfo != 0) { // parent запрашивается не у корневого элемента
        return createIndex(findRow(parentInfo), RamificationColumn, parentInfo);
    }
    else {
        return QModelIndex();
    }
}

int SimpleFSModel::findRow(const NodeInfo *nodeInfo) const
{
    const NodeInfoList& parentInfoChildren = nodeInfo->parent != 0 ? nodeInfo->parent->children: _nodes;
    NodeInfoList::const_iterator position = qFind(parentInfoChildren, *nodeInfo);
    return std::distance(parentInfoChildren.begin(), position);
}

int SimpleFSModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return _nodes.size();
    }
    const NodeInfo* parentInfo = static_cast<const NodeInfo*>(parent.internalPointer());
    return parentInfo->children.size();
}

int SimpleFSModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant SimpleFSModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const NodeInfo* nodeInfo = static_cast<NodeInfo*>(index.internalPointer());
    const QFileInfo& fileInfo = nodeInfo->fileInfo;

    switch (index.column()) {
    case NameColumn:
        return nameData(fileInfo, role);
    case ModificationDateColumn:
        if (role == Qt::DisplayRole) {
            return fileInfo.lastModified();
        }
        break;
    case SizeColumn:
        if (role == Qt::DisplayRole) {
            return fileInfo.isDir()? QVariant(): fileInfo.size();
        }
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant SimpleFSModel::nameData(const QFileInfo &fileInfo, int role) const
{
    switch (role) {
    case Qt::EditRole:
        return fileInfo.fileName();
    case Qt::DisplayRole:
        if (fileInfo.isRoot()) {
            return fileInfo.absoluteFilePath();
        }
        else if (fileInfo.isDir()){
            return fileInfo.fileName();
        }
        else {
            return fileInfo.completeBaseName();
        }
    default:
        return QVariant();
    }
    Q_UNREACHABLE();
}

bool SimpleFSModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }
    const NodeInfo* parentInfo = static_cast<const NodeInfo*>(parent.internalPointer());
    return !parentInfo->mapped;
}


void SimpleFSModel::fetchMore(const QModelIndex &parent)
{
    NodeInfo* parentInfo = static_cast<NodeInfo*>(parent.internalPointer());
    const QFileInfo& fileInfo = parentInfo->fileInfo;
    QDir dir = QDir(fileInfo.absoluteFilePath());
    QFileInfoList children = dir.entryInfoList(QStringList(), QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name);

    beginInsertRows(parent, 0, children.size() - 1);
    parentInfo->children.reserve(children.size());
    for (const QFileInfo& entry: children) {
        NodeInfo nodeInfo(entry, parentInfo);
        nodeInfo.mapped = !entry.isDir();
        parentInfo->children.push_back(std::move(nodeInfo));
    }
    parentInfo->mapped = true;
    endInsertRows();
}

void SimpleFSModel::fetchRootDirectory()
{
    const QFileInfoList drives = QDir::drives();
    qCopy(drives.begin(), drives.end(), std::back_inserter(_nodes));
}


bool SimpleFSModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        const NodeInfo* parentInfo = static_cast<const NodeInfo*>(parent.internalPointer());
        Q_ASSERT(parentInfo != 0);
        if (!parentInfo->mapped) {
            return true;//QDir(parentInfo->fileInfo.absoluteFilePath()).count() > 0; -- точное определение того, что директория не пуста
        }
    }
    return QAbstractItemModel::hasChildren(parent);
}
