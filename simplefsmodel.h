#ifndef SIMPLEFSMODEL_H
#define SIMPLEFSMODEL_H


#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QScopedPointer>
#include <QVector>

class QFileIconProvider;
class QFileInfo;

class SimpleFSModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit SimpleFSModel(QObject* parent = nullptr);
    ~SimpleFSModel();

    void setRootPath(const QString& path);

    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;

    QVariant data(const QModelIndex& index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool canFetchMore(const QModelIndex& parent) const;
    void fetchMore(const QModelIndex& parent);

    bool hasChildren(const QModelIndex& parent) const;

    QFileInfo fileInfo(const QModelIndex& index) const;

private:
    enum Columns
    {
        RamificationColumn,
        NameColumn = RamificationColumn,
        ColumnCount
    };

    struct NodeInfo;
    typedef QVector<NodeInfo> NodeInfoList;
    NodeInfoList nodes_;
    QScopedPointer<QFileIconProvider> metaProvider_;

    int findRow(const NodeInfo* nodeInfo) const;
    QVariant nameData(const QFileInfo& fileInfo, int role) const;
};

class ProxyFSModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    explicit ProxyFSModel(QObject* parent = nullptr);
    ~ProxyFSModel() override;

    void SetSourceModel(SimpleFSModel* model);
    void setRootPath(const QString& path);

    bool canFetchMore(const QModelIndex& parent) const;
    void fetchMore(const QModelIndex& parent);

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

    QModelIndex index(int row, int column,
          const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool hasChildren(const QModelIndex& parent) const;

    QFileInfo fileInfo(const QModelIndex &index) const;

private:
    QScopedPointer <SimpleFSModel> fsModel_;
};

#endif // SIMPLEFSMODEL_H
