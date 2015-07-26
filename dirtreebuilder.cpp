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
        QProgressBar* progBar, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    root_(rootPath),
    abort_(false)
{
    if (root_.isEmpty())
    {
        throw std::runtime_error("TreeBuilderThread::TreeBuilderThread: Root "
                                 "path was not setted.");
    }
}
void TreeBuilderThread::dirWasLoaded(const QString&)
{
//    emit setProgressValue(dirsWasLoaded_);
    qDebug() << "dir was loaded";
}

void TreeBuilderThread::onStart()
{
    setLabel("Building directory tree..");

    const size_t totalValue = 0;
    setProgressRange(0, totalValue);

    emit showLabel();
    emit showProgressBar();
    QEventLoop wait_loop;
    connect(&fsModel_, SIGNAL(directoryLoaded(QString)), &wait_loop, SLOT(quit()));
    connect(&fsModel_, SIGNAL(directoryLoaded(QString)), this,
            SLOT(dirWasLoaded(QString)));
    connect(this, SIGNAL(error(QString)), &wait_loop, SLOT(quit()));

    while (!abort_)
    {
        QCoreApplication::processEvents();
        wait_loop.exec();
    }

    fsModel_.setRootPath(root_);
    QThread::msleep(2000);
    qDebug() << "test 5";

    emit setProgressValue(totalValue);

    emit hideLabel();
    emit hideProgressBar();

    emit finished();
}

void TreeBuilderThread::onAbort()
{
    //TODO: проверять
    if (!abort_)
        abort_ = true;

    emit error("Abort called");
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

    TreeBuilderThread* builderThread = new TreeBuilderThread(path, progBar_, label_);

    RunThread(builderThread);
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
