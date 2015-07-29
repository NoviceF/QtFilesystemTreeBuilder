#include <cassert>
#include <stdexcept>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QEventLoop>

#include <unistd.h>

#include "dirtreebuilder.h"


TreeBuilderThread::TreeBuilderThread(const QString& rootPath,
        QProgressBar* progBar, SimpleFSModel& fsModel, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    fsModel_(fsModel),
    root_(rootPath),
    abort_(false)
{
    if (root_.isEmpty())
    {
        throw std::runtime_error("TreeBuilderThread::TreeBuilderThread: Root "
                                 "path was not setted.");
    }
}

void TreeBuilderThread::onStart()
{
    setLabel("Building directory tree..");

    emit finished();
}

///
/// \brief DirTreeBuilder
///
DirTreeBuilder::DirTreeBuilder(QObject* parent) :
    Controller(parent),
    progBar_(nullptr)
{

}

void DirTreeBuilder::BuildDirTree(const QString& path)
{
    if (IsRunning())
    {
        RiseRunningThreadWarningMsg();
        return;
    }

//    TreeBuilderThread* builderThread = new TreeBuilderThread(path, progBar_, label_);

//    RunThread(builderThread);
}

void DirTreeBuilder::onError(const QString& errorMsg)
{
    Controller::onError(errorMsg);
}

void DirTreeBuilder::onWorkDone()
{
    // do work
    Controller::onWorkDone();
}
