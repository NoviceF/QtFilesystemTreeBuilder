#include <cassert>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QString>
#include <QWidget>

#include <unistd.h>

#include "statgetter.h"

StatGetterThread::StatGetterThread(const QString& path, QProgressBar* progBar,
    QLabel* label, QTableWidget* statTable, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    path_(path),
    abort_(false),
    statTable_(statTable)
{
    if (path_.isEmpty())
        throw std::runtime_error("Path was not setted.");
}

size_t StatGetterThread::GetTotalFilesCount() const
{
    // TODO: проверить
    size_t totalCount = 0;

    for (const auto& pair : statTree_)
    {
        totalCount += pair.second.count;
    }

    return totalCount;
}

size_t StatGetterThread::GetTotalFilesSize() const
{
    // TODO: проверить
    size_t totalSize = 0;

    for (const auto& pair : statTree_)
    {
        totalSize += pair.second.size;
    }

    return totalSize;
}

size_t StatGetterThread::GetAvgSizeAllFiles() const
{
    // TODO: проверить
    return GetTotalFilesSize() / GetTotalFilesCount();

}

size_t StatGetterThread::GetTotalGroupFilesCount(const QString& groupName) const
{
    // TODO: проверить
    auto it = statTree_.find(groupName);

    if (it != statTree_.end())
        return it->second.count;

    return 0;
}

size_t StatGetterThread::GetTotalGroupFilesSize(const QString& groupName) const
{
    // TODO: проверить
    auto it = statTree_.find(groupName);

    if (it != statTree_.end())
        return it->second.size;

    return 0;
}

size_t StatGetterThread::GetAvgGroupFilesSize(const QString& groupName) const
{
    // TODO: проверить
    return GetTotalGroupFilesSize(groupName) / GetTotalGroupFilesCount(groupName);
}

size_t StatGetterThread::GetSubdirsCount()
{
    QDir rootDir(path_);
    rootDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    return rootDir.count();
}

StatGetterThread::stattree_t StatGetterThread::FillStatTreeByPath()
{
    assert(QDir::isAbsolutePath(path_));
    assert(!path_.isEmpty());

    fstree_t fsTree;

    QDirIterator it(path_, QStringList() << "*", QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext())
    {
//        qDebug() << "it next = " << it.next();
        QFileInfo fileInfo(it.next());
        fsTree[fileInfo.suffix()].push_back(fileInfo);
    }

    stattree_t result;

    for (auto pair : fsTree)
    {
        auto groupName = pair.first;
        auto groupStats = pair.second;

        const size_t elementsCount(GetTotalGroupFilesCount(groupStats));
        const size_t groupSize(GetTotalGroupFilesSize(groupStats));
        GroupStats stats = {elementsCount, groupSize};

        result.insert(std::make_pair(groupName, stats));
    }

    for (auto node : result)
    {
        qDebug() << node.first;
        qDebug() << "   " << node.second.count << " " << node.second.size;
    }

    return result;
}

/*static*/ size_t StatGetterThread::GetTotalGroupFilesCount(
        const StatGetterThread::infovec_t& infoList)
{
    return infoList.size();
}

/*static*/ size_t StatGetterThread::GetTotalGroupFilesSize(
        const StatGetterThread::infovec_t& infoList)
{
    size_t sum = 0;

    for (const QFileInfo& fileInfo : infoList)
    {
        sum += fileInfo.size();
    }

    return sum;
}

void StatGetterThread::onStart()
{
    setLabel("Calculating statistics..");

    //TODO: нужен qdiriterator и последовательный обход
    const size_t totalValue = 0;
    setProgressRange(0, totalValue);

    emit showLabel();
    emit showProgressBar();

    FillStatTreeByPath();

    emit setProgressValue(totalValue);

    emit hideLabel();
    emit hideProgressBar();

    emit finished();
}

void StatGetterThread::onAbort()
{
    //TODO: проверять
    if (!abort_)
        abort_ = true;

    emit error("Abort called");
}

///
/// \brief StatGetter
///
StatGetter::StatGetter(QObject* parent) :
    Controller(parent)
{
}

void StatGetter::GetStatsForPath(const QString& rootPath)
{
    pathInWork_ = rootPath;
    assert(!pathInWork_.isEmpty());

    if (IsRunning())
    {
        if (RiseRunningThreadWarningMsg())
           emit abort();
        else
            return;
    }

    StatGetterThread* statGetterThread =
            new StatGetterThread(rootPath, GetProgBar(), GetLabel(), tableView_);

    RunThread(statGetterThread);
}

void StatGetter::SetView(QTableWidget* view)
{
    if (!view)
        throw std::runtime_error("StatGetter::SetView: view is null.");

     tableView_ = view;
}

QTableWidget* StatGetter::GetView()
{
    assert(tableView_);
    return tableView_;
}

void StatGetter::onError(const QString& errorMsg)
{
    Controller::onError(errorMsg);
}

void StatGetter::onWorkDone()
{
    // do work
    Controller::onWorkDone();
}

