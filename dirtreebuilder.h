﻿#ifndef DIRTREEBUILDER_H
#define DIRTREEBUILDER_H

#include <QLabel>
#include <QObject>
#include <QStringList>

#include <simplefsmodel.h>
#include <controller.h>
#include <progressworker.h>


class FsModelThread : public IProgressWorker
{
    Q_OBJECT
public:
    explicit FsModelThread(const QString& rootPath, SimpleFSModel& fsModel,
            QProgressBar* progBar, QLabel* label, QObject* parent = 0);

public slots:
    virtual void onStart();

private:
    SimpleFSModel& fsModel_;
    QString root_;
    bool abort_;
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
    ProxyFSModel* proxyModel_;
};

#endif // DIRTREEBUILDER_H
