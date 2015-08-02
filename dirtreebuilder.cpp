#include <cassert>
#include <stdexcept>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QEventLoop>

#include <unistd.h>

#include "dirtreebuilder.h"

RemoteFetcherThread::RemoteFetcherThread(SimpleFSModel* fsModel,
        QProgressBar* progBar, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    fsModel_(fsModel),
    abort_(false)
{}

void RemoteFetcherThread::handle_fetchRoot(const QString& rootPath)
{
    // prepare

    FetchRoot(rootPath);
    // finalize
    emit finished();
}

void RemoteFetcherThread::handle_fetchFolder(const QModelIndex& parent)
{
    // prepare

    FetchFolder(parent);
    // finalize
    emit finished();

}

void RemoteFetcherThread::FetchRoot(const QString& rootPath)
{
    QDir pathDir(rootPath);
    pathDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    const QFileInfoList folders = pathDir.entryInfoList();
    qCopy(folders.begin(), folders.end(), std::back_inserter(fsModel_->nodes_));
}

void RemoteFetcherThread::FetchFolder(const QModelIndex& parent)
{
    Q_ASSERT(parent.isValid());
    NodeInfo* parentInfo = static_cast<NodeInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);
    Q_ASSERT(!parentInfo->mapped);

    const QFileInfo& fileInfo = parentInfo->fileInfo;
    Q_ASSERT(fileInfo.isDir());

    QDir dir = QDir(fileInfo.absoluteFilePath());

    QFileInfoList children = dir.entryInfoList(QStringList(), QDir::Dirs |
       QDir::NoDotAndDotDot, QDir::Name);

    const int insrtCnt = children.isEmpty() ?
                0 :
                children.size() - 1;

    fsModel_->beginInsertRows(parent, 0, insrtCnt);
    parentInfo->children.reserve(children.size());

    for (const QFileInfo& entry: children)
    {
        NodeInfo nodeInfo(entry, parentInfo);
        nodeInfo.mapped = !entry.isDir();
        parentInfo->children.push_back(std::move(nodeInfo));
    }

    parentInfo->mapped = true;

    fsModel_->endInsertRows();
}


///
/// \brief DirTreeBuilder
///
DirTreeBuilder::DirTreeBuilder(QObject* parent) :
    Controller(parent),
    view_(nullptr),
    fsModel_(nullptr)
{}

void DirTreeBuilder::BuildDirTree(const QString& path)
{
    if (IsRunning())
        RemoveThread();

    fsModel_ = new SimpleFSModel;

    RemoteFetcherThread* fetchThread = new RemoteFetcherThread(fsModel_, GetProgBar(),
       GetLabel());

    connect(fsModel_, SIGNAL(setRootInThread(QString)), fetchThread,
        SLOT(handle_fetchRoot(QString)));
    connect(fsModel_, SIGNAL(fetchFolderInThread(QModelIndex)), fetchThread,
            SLOT(handle_fetchFolder(QModelIndex)));

    connect(fetchThread, SIGNAL(finished()), fsModel_, SLOT(onThreadWorkDone()));

    RunThread(fetchThread);

    fsModel_->setRootPath(path);
    view_->setModel(fsModel_);
}

QString DirTreeBuilder::GetFilePathByIndex(const QModelIndex& index)
{
    if (fsModel_)
        return fsModel_->fileInfo(index).absoluteFilePath();
    else
        return QString();
}

void DirTreeBuilder::SetView(QTreeView* view)
{
    if (!view)
        throw std::runtime_error("DirTreeBuilder::SetView: view is null.");

    view_ = view;
}

void DirTreeBuilder::onWorkDone()
{
   emit closeMsgBox();
}

