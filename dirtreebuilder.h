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
    void SetProgBar(QProgressBar* progBar);
    void BuildDirTree(const QString& path);

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();

private:
    QProgressBar* progBar_;
};

#endif // DIRTREEBUILDER_H
