#ifndef DIRTREEBUILDER_H
#define DIRTREEBUILDER_H

#include <QLabel>
#include <QObject>
#include <QStringList>
#include <QTreeView>

#include <simplefsmodel.h>
#include <controller.h>
#include <progressworker.h>


class RemoteFetcherThread : public IProgressWorker
{
    Q_OBJECT

public:
    RemoteFetcherThread(SimpleFSModel* fsModel, QProgressBar* progBar,
        QLabel* label, QObject* parent = 0);

public slots:
    void handle_fetchRoot(const QString& rootPath);
    void handle_fetchFolder(const QModelIndex &parent);

private:
    void FetchRoot(const QString& rootPath);
    void FetchFolder(const QModelIndex &parent);

private:
    SimpleFSModel* fsModel_;
    bool abort_;
};


class DirTreeBuilder : public Controller
{
    Q_OBJECT

public:
    explicit DirTreeBuilder(QObject* parent = 0);
    void BuildDirTree(const QString& path);
    QString GetFilePathByIndex(const QModelIndex& index);
    void SetView(QTreeView* view);

public slots:
    virtual void onWorkDone();

private:
    QTreeView* view_;
    SimpleFSModel* fsModel_;
};

#endif // DIRTREEBUILDER_H
