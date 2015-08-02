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

    FetchRoot(rootPath);



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
    setLabel("Retrieving files count..");
    emit showLabel();
    emit setProgressRange(0, 0);
    emit setProgressValue(0);
    emit showProgressBar();

    QDir pathDir(rootPath);
    pathDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    const QFileInfoList folders = pathDir.entryInfoList();
    const int totalDirCount = folders.size();

    setLabel("Building tree..");
    emit setProgressRange(0, totalDirCount);
    emit setProgressValue(0);

    int counter = 0;

    for (QFileInfoList::const_iterator it = folders.begin(); it != folders.end();
         ++it)
    {
        fsModel_->nodes_.push_back(*it);
        ++counter;
        emit setProgressValue(counter);
    }

    emit setProgressValue(totalDirCount);

    emit hideLabel();
    emit hideProgressBar();
}

void RemoteFetcherThread::FetchFolder(const QModelIndex& parent)
{
    setLabel("Retrieving files count..");
    emit showLabel();
    emit setProgressRange(0, 0);
    emit setProgressValue(0);
    emit showProgressBar();

    Q_ASSERT(parent.isValid());
    NodeInfo* parentInfo = static_cast<NodeInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);
    Q_ASSERT(!parentInfo->mapped);

    const QFileInfo& fileInfo = parentInfo->fileInfo;
    Q_ASSERT(fileInfo.isDir());

    QDir dir = QDir(fileInfo.absoluteFilePath());

    QFileInfoList children = dir.entryInfoList(QStringList(), QDir::Dirs |
       QDir::NoDotAndDotDot, QDir::Name);

    const int childrenCount = children.size();

    setLabel("Building tree..");
    emit setProgressRange(0, childrenCount);
    emit setProgressValue(0);

    int counter = 0;

    const int insrtCnt = children.isEmpty() ?
                0 :
                childrenCount - 1;

    fsModel_->beginInsertRows(parent, 0, insrtCnt);
    parentInfo->children.reserve(children.size());

    for (const QFileInfo& entry: children)
    {
        NodeInfo nodeInfo(entry, parentInfo);
        nodeInfo.mapped = !entry.isDir();
        parentInfo->children.push_back(std::move(nodeInfo));

        ++counter;
        emit setProgressValue(counter);
    }

    parentInfo->mapped = true;

    fsModel_->endInsertRows();

    emit setProgressValue(childrenCount);

    emit hideLabel();
    emit hideProgressBar();
}


///
/// \brief DirTreeBuilder
///
DirTreeBuilder::DirTreeBuilder(QObject* parent) :
    Controller(parent),
    view_(nullptr)
{}

void DirTreeBuilder::BuildDirTree(const QString& path)
{
    if (IsRunning())
        RemoveThread();

    fsModel_.reset(new SimpleFSModel);

    RunThread(CreateFetchThread());

    fsModel_->setRootPath(path);
    view_->setModel(fsModel_.data());
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

RemoteFetcherThread*DirTreeBuilder::CreateFetchThread()
{
    fetchThread_.reset(new RemoteFetcherThread(fsModel_.data(),
        GetProgBar(), GetLabel()));

    connect(fsModel_.data(), SIGNAL(setRootInThread(QString)), fetchThread_.data(),
        SLOT(handle_fetchRoot(QString)));
    connect(fsModel_.data(), SIGNAL(fetchFolderInThread(QModelIndex)),
            fetchThread_.data(), SLOT(handle_fetchFolder(QModelIndex)));

    connect(fetchThread_.data(), SIGNAL(finished()), fsModel_.data(),
        SLOT(onThreadWorkDone()));

    return fetchThread_.data();
}

void DirTreeBuilder::onWorkDone()
{
   emit closeMsgBox();
}

