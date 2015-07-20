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
    size_t result = 0;

//    for (const auto& pair : sortTree_)
//    {
//        result += pair.second.count();
//    }

    return result;
}

size_t FileTreeAnalyzer::GetTotalFilesSize() const
{
    size_t result = 0;

//    for (const auto& pair : sortTree_)
//    {
//        for (const QFileInfo& fileInfo : pair.second)
//        {
//            result += fileInfo.size();
//        }
//    }

    return result;
}

size_t FileTreeAnalyzer::GetAvgFilesSize() const
{

}

size_t FileTreeAnalyzer::GetTotalGroupFilesCount(const QString& groupName) const
{

}

size_t FileTreeAnalyzer::GetTotalGroupFilesSize(const QString& groupName) const
{

}

size_t FileTreeAnalyzer::GetAvgGroupFilesSize(const QString& groupName) const
{

}

size_t FileTreeAnalyzer::GetSubdirsCount()
{

}

FileTreeAnalyzer::stattree_t FileTreeAnalyzer::GetTreeFilledByRoot()
{
    assert(QDir::isAbsolutePath(root_));
    assert(!root_.isEmpty());

    fstree_t fsTree;

    QDirIterator it(root_, QDirIterator::Subdirectories);

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

    return result;
}

/*static*/ size_t FileTreeAnalyzer::GetTotalGroupFilesCount(const QString& groupName,
    const FileTreeAnalyzer::infovec_t& tree)
{

}

/*static*/ size_t FileTreeAnalyzer::GetTotalGroupFilesSize(const QString& groupName,
    const FileTreeAnalyzer::infovec_t& tree)
{

}























