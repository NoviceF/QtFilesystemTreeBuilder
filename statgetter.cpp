#include <cassert>

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QString>
#include <QWidget>

#include "statgetter.h"

StatGetterThread::StatGetterThread(const QString& path, StatsCont& statCont,
    QProgressBar* progBar, QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    path_(path),
    subdirsInPathDir_(statCont.subdirsCount),
    statTree_(statCont.statTree)
{
    if (path_.isEmpty())
    {
        throw std::runtime_error("StatGetterThread::StatGetterThread: Path "
                                 "was not setted.");
    }
}

void StatGetterThread::GetSubdirsCount() const
{
    subdirsInPathDir_ = 0;

    QDir rootDir(path_);
    rootDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    subdirsInPathDir_ = rootDir.count();
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

size_t StatGetterThread::GetTotalGroupFilesCount(
        const infovec_t& infoList) const
{
    return infoList.size();
}

size_t StatGetterThread::GetTotalGroupFilesSize(
        const infovec_t& infoList) const
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
    setLabel("Retrieving files count..");
    emit showLabel();
    emit setProgressRange(0, 0);
    emit setProgressValue(0);
    emit showProgressBar();

    FillPreAnalysisTree();
    const size_t totalValue = preAnalysisTree_.size();

    setLabel("Calculating statistics..");
    emit setProgressRange(0, totalValue);
    emit setProgressValue(0);

    FillStatTreeByPath();
    GetSubdirsCount();

    emit setProgressValue(totalValue);

    emit hideLabel();
    emit hideProgressBar();

    emit finished();
}

///
/// \brief StatGetter
///

StatGetter::StatGetter(QObject* parent) :
    Controller(parent),
    tableWidget_(nullptr),
    subdirsInCurPathCount_(0)
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
    subdirsInCurPathCount_ = 0;
    StatsCont cont {statTree_, subdirsInCurPathCount_};

    currentThreadClass_.reset(new StatGetterThread(rootPath, cont,
        GetProgBar(), GetLabel()) );
    connect(&GetWorkerThread(), SIGNAL(started()), currentThreadClass_.data(),
        SLOT(onStart()));

    RunThread(currentThreadClass_.data());
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
    const int totalFilesCount = GetTotalFilesCount();

    if (totalFilesCount == 0)
        return 0;

    return GetTotalFilesSize() / totalFilesCount;
}

void StatGetter::FillWidgetTable()
{
    tableWidget_->setColumnCount(2);
    tableWidget_->setHorizontalHeaderLabels(QStringList() << "Name" << "Value");
    // количество групп + общая статистика + количество каталогов в текущем
    const int groupCount = statTree_.size() + 1;
    const int blankLineCount = groupCount - 1;
    const int rowOnGroup = 4;
    const int spaceForDirsInCurDir = 2;
    const int rowCount = groupCount * rowOnGroup + blankLineCount +
            spaceForDirsInCurDir;
    tableWidget_->setRowCount(rowCount);

    int rowNumber = 0;

    for (auto statPair : statTree_)
    {
        const QString groupName = statPair.first.isEmpty() ?
                    "*no extention*" :
                    "*." + statPair.first;

        AddTableRow(rowNumber, groupName);
        AddTableRow(rowNumber, "Files count", &statPair.second.count);
        AddTableRow(rowNumber, "Files size", &statPair.second.size);
        const size_t avgSize = statPair.second.size / statPair.second.count;
        AddTableRow(rowNumber, "Avg size", &avgSize);
        ++rowNumber;
    }

    AddTableRow(rowNumber, "Total");
    const size_t totalFilesCount = GetTotalFilesCount();
    AddTableRow(rowNumber, "Total files count", &totalFilesCount);
    const size_t totalFilesSize = GetTotalFilesSize();
    AddTableRow(rowNumber, "Total files count", &totalFilesSize);
    const size_t avgSizeAllFiles = GetAvgSizeAllFiles();
    AddTableRow(rowNumber, "Avg by all files size", &avgSizeAllFiles);
    ++rowNumber;

    AddTableRow(rowNumber, "Subdirs count", &subdirsInCurPathCount_);
}

void StatGetter::AddTableRow(int& rowNumber, const QString& name,
                             const size_t* value /*= nullptr*/)
{
    const int nameCol = 0;
    const int valueCol = 1;

    tableWidget_->setItem(rowNumber, nameCol, new QTableWidgetItem(name));

    if (value)
    {
        const QString stringValue = QString::number(*value);
        tableWidget_->setItem(rowNumber, valueCol,
                              new QTableWidgetItem(stringValue));
    }

    ++rowNumber;
}

void StatGetter::onWorkDone()
{
    FillWidgetTable();
    Controller::onWorkDone();
}
