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
    abort_(false),
    dirsWasLoaded_(0),
    dirList_(GetDirList())
{
    if (root_.isEmpty())
        throw std::runtime_error("Root path was not setted.");
}
void TreeBuilderThread::dirWasLoaded(const QString&)

{
    ++dirsWasLoaded_;
    emit setProgressValue(dirsWasLoaded_);
    qDebug() << "dir was loaded";
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
    const size_t totalValue = dirList_.count();
    setProgressRange(0, totalValue);

    emit showLabel();
    emit showProgressBar();
    QEventLoop wait_loop;
    connect(&fsModel_, SIGNAL(directoryLoaded(QString)), &wait_loop, SLOT(quit()));
    connect(&fsModel_, SIGNAL(directoryLoaded(QString)), this,
            SLOT(dirWasLoaded(QString)));
    connect(this, SIGNAL(error(QString)), &wait_loop, SLOT(quit()));

    while ((dirsWasLoaded_ != totalValue) && !abort_)
    {
        for (const QString& path : dirList_)
        {
            qDebug() << "test 1";
            QModelIndex index = fsModel_.index(path);
            if (fsModel_.canFetchMore(index))
            {
                fsModel_.fetchMore(index);
            qDebug() << "test 2";
            }

            QCoreApplication::processEvents();
            qDebug() << "test 3";
        }

        qDebug() << "test 4";
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

QStringList TreeBuilderThread::GetDirList()
{
    QDirIterator it(root_, QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    QStringList dirList;

    while (it.hasNext())
        dirList.push_back(it.next());

    return dirList;
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
