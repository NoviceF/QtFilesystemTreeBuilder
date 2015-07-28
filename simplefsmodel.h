#ifndef SIMPLEFSMODEL_H
#define SIMPLEFSMODEL_H


#include <QAbstractItemModel>
#include <QVector>

class QFileIconProvider;
class QFileInfo;

class SimpleFSModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    SimpleFSModel(QObject* parent);
    ~SimpleFSModel();

    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;

    QVariant data(const QModelIndex& index, int role) const;

    bool canFetchMore(const QModelIndex& parent) const;
    void fetchMore(const QModelIndex& parent);

    bool hasChildren(const QModelIndex& parent) const;

private:
    enum Columns
    {
        RamificationColumn,
        NameColumn = RamificationColumn,
        ModificationDateColumn,
        SizeColumn,
        TypeColumn,
        ColumnCount
    };

    struct NodeInfo;
    typedef QVector<NodeInfo> NodeInfoList;
    NodeInfoList nodes_;
    QScopedPointer<QFileIconProvider> metaProvider_;

    void fetchRootDirectory();
    int findRow(const NodeInfo* nodeInfo) const;
    QVariant nameData(const QFileInfo& fileInfo, int role) const;
};

#endif // SIMPLEFSMODEL_H
