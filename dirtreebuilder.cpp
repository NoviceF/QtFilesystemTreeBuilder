#include <cassert>
#include <stdexcept>

#include <QDir>
#include <QDirIterator>
#include <QEventLoop>

#include <unistd.h>

#include "dirtreebuilder.h"


TreeBuilderThread::TreeBuilderThread(const QString& rootPath,
        QProgressBar* progBar, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    root_(rootPath),
    abort_(false),
    dirWasLoaded_(0)
{
    if (root_.isEmpty())
        throw std::runtime_error("Root path was not setted.");
}

size_t TreeBuilderThread::GetTotalDirsCount()
{
    QDirIterator it(root_, QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    size_t counter = 0;

    while (it.hasNext())
    {
        it.next();
        ++counter;
    }

    return counter;
}

void TreeBuilderThread::dirWasLoaded(const QString&)
{
    ++dirWasLoaded_;
}

//    for (int i = 0; i < totalValue && !abort_; i += totalValue / 5)
//    {
//        emit setProgressValue(i);
//        sleep(1);
//    }


void TreeBuilderThread::onStart()
{
    setLabel("Building directory tree..");

    //TODO: нужен qdiriterator и последовательный обход
    const size_t totalValue = GetTotalDirsCount();
    setProgressRange(0, totalValue);

    emit showLabel();
    emit showProgressBar();
    QEventLoop wait_loop;
    connect(&fsModel_, SIGNAL(directoryLoaded(QString)), &wait_loop, SLOT(quit()));
    connect(&fsModel_, SIGNAL(directoryLoaded(QString)), this,
            SLOT(dirWasLoaded(QString)));
    connect(this, SIGNAL(error(QString)), &wait_loop, SLOT(quit()));

    fsModel_.setRootPath(root_);

    while ((dirWasLoaded_ != totalValue) && !abort_)
    {
        wait_loop.exec();
    }

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

void DirTreeBuilder::SetProgBar(QProgressBar* progBar)
{
    progBar_ = progBar;
}

void DirTreeBuilder::SetLabel(QLabel* label)
{
    label_ = label;
}

void DirTreeBuilder::BuildDirTree(const QString& path)
{
    assert(progBar_ && label_);

    if (!progBar_ || !label_)
    {
        throw std::runtime_error("DirTreeBuilder::BuildDirTree: Progress bar and "
             "label must be set before use.");
    }

    if (IsRunning())
    {
        if (RiseRunningThreadWarningMsg())
           emit abort();
        else
            return;
    }

    TreeBuilderThread* builderThread = new TreeBuilderThread(path, progBar_, label_);
    connect(this, SIGNAL(abort()), builderThread, SLOT(onAbort()));
    connect(builderThread, SIGNAL(error(QString)), this, SLOT(onError(QString)));
    connect(builderThread, SIGNAL(finished()), this, SLOT(onWorkDone()));

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
