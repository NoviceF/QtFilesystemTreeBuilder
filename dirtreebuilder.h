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
    enum class FetchType
    {
        FetchRoot = 0,
        FetchFolder,
        NoFetch
    };

    RemoteFetcherThread(const QString& rootPath, SimpleFSModel* fsModel,
            QProgressBar* progBar, QLabel* label, QObject* parent = 0);

private:
    void FetchRoot();
    void FetchFolder(const QString& path);

public slots:
    virtual void onStart();
    virtual void setFetchParams(FetchType type, const QString& path = QString());


private:
    SimpleFSModel* fsModel_;
    QString root_;
    bool abort_;
    FetchType fetchType_;
};

class DirTreeBuilder : public Controller
{
    Q_OBJECT

public:
    explicit DirTreeBuilder(QObject* parent = 0);
    void BuildDirTree(const QString& path);

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();

private:
    QProgressBar* progBar_;
    QLabel* label_;
};

#endif // DIRTREEBUILDER_H
