#ifndef DIRTREEBUILDER_H
#define DIRTREEBUILDER_H

#include <QObject>

#include <controller.h>
#include <progressworker.h>


class TreeBuilderThreed : public IProgressWorker
{
    Q_OBJECT
public:
    explicit TreeBuilderThreed(const QString& rootPath, QProgressBar* progBar,
        QObject* parent = 0);

public slots:
    virtual void onStart();
    virtual void onAbort();

private:
    QString root_;
    bool abort_;

};

class DirTreeBuilder : public Controller
{
    Q_OBJECT
public:
    explicit DirTreeBuilder(QObject* parent = 0);
    void BuildDirTree(TreeBuilderThreed* builderThread);

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();
};

#endif // DIRTREEBUILDER_H
