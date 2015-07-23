#include <cassert>
#include <stdexcept>

#include <QDir>
#include <QDirIterator>

#include <unistd.h>

#include "dirtreebuilder.h"


TreeBuilderThreed::TreeBuilderThreed(const QString& rootPath,
        QProgressBar* progBar, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    root_(rootPath),
    abort_(false)
{
    if (root_.isEmpty())
        throw std::runtime_error("Root path was not setted.");
}

void TreeBuilderThreed::onStart()
{
    setLabel("Building directory tree..");

//    QDirModel* model = new QDirModel(QStringList() << "*",
//         QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
    QDir dir("/home/novice/proj/cpp/dirtest");
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    //TODO: нужен qdiriterator и последовательный обход
    const int totalValue = dir.entryList().count();
    setProgressRange(0, totalValue);

    emit showLabel();
    emit showProgressBar();

//    for (int i = 0; i < totalValue && !abort_; i += totalValue / 5)
//    {
//        emit setProgressValue(i);
//        sleep(1);
//    }



    emit setProgressValue(totalValue);

    emit hideLabel();
    emit hideProgressBar();

    emit finished();
}

void TreeBuilderThreed::onAbort()
{
    //TODO: проверять
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

    TreeBuilderThreed* builderThread = new TreeBuilderThreed(path, progBar_, label_);
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
