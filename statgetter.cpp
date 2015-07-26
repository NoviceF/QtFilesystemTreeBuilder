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

size_t GetTotalGroupFilesCount(
        const infovec_t& infoList)
{
    return infoList.size();
}

size_t GetTotalGroupFilesSize(
        const infovec_t& infoList)
{
    size_t sum = 0;

    for (const QFileInfo& fileInfo : infoList)
    {
        sum += fileInfo.size();
    }

    return sum;
}

StatGetterThread::StatGetterThread(const QString& path, stattree_t& statTree,
    QProgressBar* progBar, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    path_(path),
    statTree_(statTree)
{
    if (path_.isEmpty())
    {
        throw std::runtime_error("StatGetterThread::StatGetterThread: Path "
                                 "was not setted.");
    }
}

void StatGetterThread::FillPreAnalysisTree()
{
    assert(!path_.isEmpty());

    QDirIterator it(path_, QStringList() << "*", QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QCoreApplication::processEvents();
        QFileInfo fileInfo(it.next());
        preAnalysisTree_[fileInfo.suffix()].push_back(fileInfo);
    }
}

void StatGetterThread::FillStatTreeByPath()
{
    int counter = 0;

    for (auto pair : preAnalysisTree_)
    {
        auto groupName = pair.first;
        auto groupStats = pair.second;

        const size_t elementsCount(GetTotalGroupFilesCount(groupStats));
        const size_t groupSize(GetTotalGroupFilesSize(groupStats));
        GroupStats stats = {elementsCount, groupSize};

        statTree_.insert(std::make_pair(groupName, stats));

        ++counter;
        emit setProgressValue(counter);
    }
}

void StatGetterThread::onStart()
{
    setLabel("Retrieving files count..");
    emit showLabel();
    FillPreAnalysisTree();
    const size_t totalValue = preAnalysisTree_.size();

    setLabel("Calculating statistics..");
    setProgressRange(0, totalValue);

    emit setProgressValue(0);
    emit showProgressBar();

    FillStatTreeByPath();

    emit setProgressValue(totalValue);

    emit hideLabel();
    emit hideProgressBar();

    emit finished();
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
        RiseRunningThreadWarningMsg();
        return;
    }

    statTree_.clear();
    StatGetterThread* statGetterThread = new StatGetterThread(rootPath, statTree_,
        GetProgBar(), GetLabel());

    RunThread(statGetterThread);
}

void StatGetter::SetView(QTableWidget* view)
{
    if (!view)
        throw std::runtime_error("StatGetter::SetView: view is null.");

    tableWidget_ = view;
}

size_t StatGetter::GetTotalFilesCount() const
{
    size_t totalCount = 0;

    for (const auto& pair : statTree_)
    {
        totalCount += pair.second.count;
    }

    return totalCount;
}


size_t StatGetter::GetTotalFilesSize() const
{
    size_t totalSize = 0;

    for (const auto& pair : statTree_)
    {
        totalSize += pair.second.size;
    }

    return totalSize;
}

size_t StatGetter::GetAvgSizeAllFiles() const
{
    return GetTotalFilesSize() / GetTotalFilesCount();

}

//size_t StatGetter::GetTotalGroupFilesCount(const QString& groupName) const
//{
//    auto it = statTree_.find(groupName);

//    if (it != statTree_.end())
//        return it->second.count;

//    return 0;
//}

//size_t StatGetter::GetTotalGroupFilesSize(const QString& groupName) const
//{
//    auto it = statTree_.find(groupName);

//    if (it != statTree_.end())
//        return it->second.size;

//    return 0;
//}

//size_t StatGetter::GetAvgGroupFilesSize(const QString& groupName) const
//{
//    return GetTotalGroupFilesSize(groupName) / GetTotalGroupFilesCount(groupName);
//}

size_t StatGetter::GetSubdirsCount()
{
    QDir rootDir(pathInWork_);
    rootDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    return rootDir.count();
}

void StatGetter::FillWidgetTable()
{
    tableWidget_->setColumnCount(2);
    tableWidget_->setHorizontalHeaderLabels(QStringList() << "Name" << "Value");
    // количество групп + общая статистика
    const int groupCount = statTree_.size() + 1;
    const int blankLineCount = groupCount - 1;
    const int rowOnGroup = 4;
    const int rowCount = groupCount * rowOnGroup + blankLineCount;
    tableWidget_->setRowCount(rowCount);

    for (auto it : statTree_)
    {

    }

}

void StatGetter::onError(const QString& errorMsg)
{
    Controller::onError(errorMsg);
}

void StatGetter::onWorkDone()
{
    FillWidgetTable();
    Controller::onWorkDone();
}

