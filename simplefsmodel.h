#ifndef SIMPLEFSMODEL_H
#define SIMPLEFSMODEL_H

#include <QAbstractItemModel>
#include <QFileInfo>
#include <QVector>

class QFileIconProvider;

struct NodeInfo
{
    NodeInfo():
        parent(nullptr),
        mapped(false)
    {}

    NodeInfo(const QFileInfo& fileInfo, NodeInfo* parent = nullptr):
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

class SimpleFSModel : public QAbstractItemModel
{
    Q_OBJECT

    friend class RemoteFetcherThread;

public:
    explicit SimpleFSModel(QObject* parent = nullptr);
    ~SimpleFSModel();

    void setRootPath(const QString& path);

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    bool hasChildren(const QModelIndex& parent) const override;

    QFileInfo fileInfo(const QModelIndex& index) const;

signals:
    void setRootInThread(const QString& root);
    void fetchFolderInThread(const QModelIndex& index);
    void breakWaitLoop();

public slots:
    void onThreadWorkDone();

private:
    int findRow(const NodeInfo* nodeInfo) const;
    QVariant nameData(const QFileInfo& fileInfo, int role) const;

private:
    enum Columns
    {
        RamificationColumn,
        NameColumn = RamificationColumn,
        ColumnCount
    };

    typedef QVector<NodeInfo> NodeInfoList;
    NodeInfoList nodes_;
    QScopedPointer<QFileIconProvider> metaProvider_;
};

#endif // SIMPLEFSMODEL_H
