#include <QDir>
#include <QEventLoop>
#include <QFileIconProvider>

#include "simplefsmodel.h"

SimpleFSModel::SimpleFSModel(QObject* parent) :
    QAbstractItemModel(parent),
    metaProvider_(new QFileIconProvider())
{
}

SimpleFSModel::~SimpleFSModel()
{}

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

void SimpleFSModel::onThreadWorkDone()
{
    emit breakWaitLoop();
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
        return QVariant();
    }
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

void SimpleFSModel::fetchMore(const QModelIndex& parent)
{
    QEventLoop waitLoop;
    connect(this, SIGNAL(breakWaitLoop()), &waitLoop, SLOT(quit()));

    emit fetchFolderInThread(parent);

    waitLoop.exec();
}

void SimpleFSModel::setRootPath(const QString& path)
{
    QEventLoop waitLoop;
    connect(this, SIGNAL(breakWaitLoop()), &waitLoop, SLOT(quit()));

    emit setRootInThread(path);

    waitLoop.exec();
}







