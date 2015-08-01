#ifndef DIRTREEBUILDER_H
#define DIRTREEBUILDER_H

#include <QLabel>
#include <QObject>
#include <QStringList>

#include <simplefsmodel.h>
#include <controller.h>
#include <progressworker.h>


class RemoteFetcherThread : public IProgressWorker
{
    Q_OBJECT

public:
    RemoteFetcherThread(const QString& rootPath, SimpleFSModel* fsModel,
            QProgressBar* progBar, QLabel* label, QObject* parent = 0);

public slots:
    void FetchRoot();
    void FetchFolder(const QString& path);

private:
    SimpleFSModel* fsModel_;
    QString root_;
    bool abort_;
};


class DirTreeBuilder : public Controller
{
    Q_OBJECT

public:
    explicit DirTreeBuilder(QObject* parent = 0);
    void BuildDirTree(const QString& path);
    QString GetFilePathByIndex(const QModelIndex& index);

private:
    SimpleFSModel* fsModel_;
};

#endif // DIRTREEBUILDER_H
