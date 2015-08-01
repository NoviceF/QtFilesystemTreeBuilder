#include <cassert>
#include <stdexcept>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QEventLoop>

#include <unistd.h>

#include "dirtreebuilder.h"

RemoteFetcherThread::RemoteFetcherThread(const QString& rootPath,
    SimpleFSModel* fsModel, QProgressBar* progBar, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    fsModel_(fsModel),
    root_(rootPath),
    abort_(false),
    fetchType_(FetchType::NoFetch)
{
    if (root_.isEmpty())
    {
        throw std::runtime_error("TreeBuilderThread::TreeBuilderThread: Root "
                                 "path was not setted.");
    }
}


///
/// \brief DirTreeBuilder
///
DirTreeBuilder::DirTreeBuilder(QObject* parent) :
    Controller(parent),
    fsModel_(nullptr)
{}

void DirTreeBuilder::BuildDirTree(const QString& path)
{
    if (IsRunning())
        RemoveThread();

    SimpleFSModel* fsModel = new SimpleFSModel;
    fsModel->setRootPath(path);

}

QString DirTreeBuilder::GetFilePathByIndex(const QModelIndex& index)
{
    if (fsModel_)
        return fsModel_->fileInfo(index).absoluteFilePath();
    else
        return QString();
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
