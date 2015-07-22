#include <cassert>

#include <QDebug>
#include <QDirIterator>

#include "filetreeanalyzer.h"

// Подписаться где-нибунь на сигнал изменения файловой системы, и дальше не
// использовать данный класс.
FileTreeAnalyzer::FileTreeAnalyzer(const QString &root) :
    root_(root),
    statTree_(GetTreeFilledByRoot())
{
    assert(QDir::isAbsolutePath(root_));
}

size_t FileTreeAnalyzer::GetTotalFilesCount() const
{
    // TODO: проверить
    size_t totalCount = 0;

    for (const auto& pair : statTree_)
    {
        totalCount += pair.second.count;
    }

    return totalCount;
}

size_t FileTreeAnalyzer::GetTotalFilesSize() const
{
    // TODO: проверить
    size_t totalSize = 0;

    for (const auto& pair : statTree_)
    {
        totalSize += pair.second.size;
    }

    return totalSize;
}

size_t FileTreeAnalyzer::GetAvgSizeAllFiles() const
{
    // TODO: проверить
    return GetTotalFilesSize() / GetTotalFilesCount();

}

size_t FileTreeAnalyzer::GetTotalGroupFilesCount(const QString& groupName) const
{
    // TODO: проверить
//    return statTree_[groupName].count;
    return statTree_.find(groupName).count;

    auto it =
}

size_t FileTreeAnalyzer::GetTotalGroupFilesSize(const QString& groupName) const
{
    // TODO: проверить
    return statTree_.find(groupName).size;

}

size_t FileTreeAnalyzer::GetAvgGroupFilesSize(const QString& groupName) const
{
    // TODO: проверить
    return GetTotalGroupFilesSize(groupName) / GetTotalGroupFilesCount(groupName);
}

size_t FileTreeAnalyzer::GetSubdirsCount()
{
    QDir rootDir(root_);
    rootDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    return rootDir.count();
}

FileTreeAnalyzer::stattree_t FileTreeAnalyzer::GetTreeFilledByRoot()
{
    assert(QDir::isAbsolutePath(root_));
    assert(!root_.isEmpty());

    fstree_t fsTree;

    QDirIterator it(root_, QStringList() << "*", QDir::Files,
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

        const size_t elementsCount(GetTotalGroupFilesCount(groupName, groupStats));
        const size_t groupSize(GetTotalGroupFilesSize(groupName, groupStats));
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

/*static*/ size_t FileTreeAnalyzer::GetTotalGroupFilesCount(const QString& groupName,
    const FileTreeAnalyzer::infovec_t& infoList)
{
    return infoList.size();
}

/*static*/ size_t FileTreeAnalyzer::GetTotalGroupFilesSize(const QString& groupName,
    const FileTreeAnalyzer::infovec_t& infoList)
{
    size_t sum = 0;

    for (const QFileInfo& fileInfo : infoList)
    {
        sum += fileInfo.size();
    }

    return sum;
}























