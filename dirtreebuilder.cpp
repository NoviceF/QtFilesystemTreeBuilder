#include <cassert>
#include <stdexcept>

#include <unistd.h>

#include "dirtreebuilder.h"


TreeBuilderThreed::TreeBuilderThreed(const QString& rootPath,
        QProgressBar* progBar, QObject* parent) :
    IProgressWorker(progBar, parent),
    root_(rootPath),
    abort_(false)
{
    if (root_.isEmpty())
        throw std::runtime_error("Root path was not setted.");
}

void TreeBuilderThreed::onStart()
{

    const int totalValue = 50;
    setProgressRange(0, totalValue);

    for (int i = 0; i < totalValue && !abort_; i += totalValue / 5)
    {
        emit setProgressValue(i);
        sleep(2);
    }

    emit finished();
}

void TreeBuilderThreed::onAbort()
{
    if (!abort_)
        abort_ = true;
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

void DirTreeBuilder::BuildDirTree(const QString& path)
{
    assert(progBar_);

    if (!progBar_)
        throw std::runtime_error("Progress bar must be set before use.");

    if (IsRunning())
    {
        if (RiseRunningThreadWarningMsg())
           emit abort();
        else
            return;
    }

    TreeBuilderThreed* builderThread = new TreeBuilderThreed(path, progBar_, this);
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
