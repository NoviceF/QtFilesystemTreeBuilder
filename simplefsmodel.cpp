#include <QFileInfo>
#include <QDir>
#include <algorithm>
#include <QFileIconProvider>
#include <QDateTime>
#include <QDebug>

#include "simplefsmodel.h"

SimpleFSModel::SimpleFSModel(QObject* parent) :
    QAbstractItemModel(parent),
    metaProvider_(new QFileIconProvider())
{
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

QModelIndex SimpleFSModel::index(int row, int column,
     const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    if (!parent.isValid()) // запрашивают индексы корневых узлов
    {
        Q_ASSERT(nodes_.size() > row);
        return createIndex(row, column, const_cast<NodeInfo*>(&nodes_[row]));
    }

    NodeInfo* parentInfo = static_cast<NodeInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);
    Q_ASSERT(parentInfo->mapped);
    Q_ASSERT(parentInfo->children.size() > row);
    return createIndex(row, column, &parentInfo->children[row]);
}

QModelIndex SimpleFSModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    NodeInfo* childInfo = static_cast<NodeInfo*>(child.internalPointer());
    Q_ASSERT(childInfo != 0);
    NodeInfo* parentInfo = childInfo->parent;

    if (parentInfo != 0)
        return createIndex(findRow(parentInfo), RamificationColumn, parentInfo);
    else
        return QModelIndex();
}

int SimpleFSModel::findRow(const NodeInfo* nodeInfo) const
{
    Q_ASSERT(nodeInfo != 0);
    const NodeInfoList& parentInfoChildren = nodeInfo->parent != 0 ?
                nodeInfo->parent->children :
                nodes_;
    NodeInfoList::const_iterator position = qFind(parentInfoChildren, *nodeInfo);
    Q_ASSERT(position != parentInfoChildren.end());
    return std::distance(parentInfoChildren.begin(), position);
}

int SimpleFSModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return nodes_.size();

    const NodeInfo* parentInfo =
            static_cast<const NodeInfo*>(parent.internalPointer());

    Q_ASSERT(parentInfo != 0);

    return parentInfo->children.size();
}

bool SimpleFSModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        const NodeInfo* parentInfo =
                static_cast<const NodeInfo*>(parent.internalPointer());
        Q_ASSERT(parentInfo != 0);

        if (!parentInfo->mapped)
            return QDir(parentInfo->fileInfo.absoluteFilePath()).count() > 0;
    }

    return QAbstractItemModel::hasChildren(parent);
}

QFileInfo SimpleFSModel::fileInfo(const QModelIndex& index) const
{
    return static_cast<NodeInfo*>(index.internalPointer())->fileInfo;
}

int SimpleFSModel::columnCount(const QModelIndex&) const
{
    return ColumnCount;
}

QVariant SimpleFSModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const NodeInfo* nodeInfo = static_cast<NodeInfo*>(index.internalPointer());
    const QFileInfo& fileInfo = nodeInfo->fileInfo;
    Q_ASSERT(nodeInfo != 0);

    switch (index.column())
    {
    case NameColumn:
        return nameData(fileInfo, role);
    default:
        break;
    }

    return QVariant();
}

QVariant SimpleFSModel::nameData(const QFileInfo &fileInfo, int role) const
{

    switch (role)
    {
        case Qt::DisplayRole:
        {
            if (fileInfo.isRoot())
                return fileInfo.absoluteFilePath();
            else
                return fileInfo.fileName();
        }
        case Qt::DecorationRole:
        {
            return metaProvider_->icon(fileInfo);
        }
        default:
        {
            return QVariant();
        }
    }

    Q_UNREACHABLE();
}

QVariant SimpleFSModel::headerData(int section, Qt::Orientation orientation,
       int role) const
{
    const QStringList headers = {"Name"};
    if (orientation == Qt::Horizontal && role ==
            Qt::DisplayRole && section < headers.size())
    {
        return headers[section];
    }

    return QVariant();
}

bool SimpleFSModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return false;

    const NodeInfo* parentInfo =
            static_cast<const NodeInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);

    return !parentInfo->mapped;
}

void SimpleFSModel::fetchMore(const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid());
    NodeInfo* parentInfo = static_cast<NodeInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);
    Q_ASSERT(!parentInfo->mapped);

    const QFileInfo& fileInfo = parentInfo->fileInfo;
    Q_ASSERT(fileInfo.isDir());

    QDir dir = QDir(fileInfo.absoluteFilePath());

    QFileInfoList children = dir.entryInfoList(QStringList(), QDir::Dirs |
       QDir::NoDotAndDotDot, QDir::Name);

    parentInfo->mapped = true;

    if (children.isEmpty())
        return;

    beginInsertRows(parent, 0, children.size() - 1);
    parentInfo->children.reserve(children.size());

    for (const QFileInfo& entry: children)
    {
        NodeInfo nodeInfo(entry, parentInfo);
        nodeInfo.mapped = !entry.isDir();
        parentInfo->children.push_back(std::move(nodeInfo));
    }

    endInsertRows();
}

void SimpleFSModel::setRootPath(const QString& path)
{
//    const QFileInfoList drives = QDir::drives();
    QDir pathDir(path);
    pathDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    const QFileInfoList drives = pathDir.entryInfoList();
    qCopy(drives.begin(), drives.end(), std::back_inserter(nodes_));
}

