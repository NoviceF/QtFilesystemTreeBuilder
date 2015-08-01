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

void RemoteFetcherThread::onStart()
{
    setLabel("Building directory tree..");

    emit finished();
}

void RemoteFetcherThread::setFetchParams(RemoteFetcherThread::FetchType type,
    const QString& path)
{
    switch (type)
    {
        case FetchType::FetchRoot :
            FetchRoot();
            break;
        case FetchType::FetchFolder :
            FetchFolder(path);
            break;
        default:
            return;
    }
}

///
/// \brief DirTreeBuilder
///
DirTreeBuilder::DirTreeBuilder(QObject* parent) :
    Controller(parent),
    progBar_(nullptr),
    label_(nullptr)
{
}

void DirTreeBuilder::BuildDirTree(const QString& path)
{
    if (IsRunning())
        RemoveThread();

//    // fsProxyModel_ ждёт в евент лупе, пока не придёт сигнал что реальная модель выполнила обработку
//    // запроса, после чего обращается к обычной модели напрямую
//    proxyModel_->SetSourceModel(new SimpleFSModel);
//    // Нужно удалить fsSimpleModel перед установкой нового
//    FsModelThread* builderThread = new FsModelThread(fsSimpleModel, progBar_, label_);
//    RunThread(builderThread);
//    // прокси шлёт сигналы на действия, которые модифицируют модель и ждёт ответа - подтверждения
//    // после их завершения. Функции же, не модифицирующие объект, могут использоваться напрямую
//    fsProxyModel_->setRootPath(path);
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
