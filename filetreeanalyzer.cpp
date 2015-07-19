#include <cassert>

#include <QDebug>
#include <QDir>

#include "filetreeanalyzer.h"

// Подписаться где-нибунь на сигнал изменения файловой системы, и дальше не
// использовать данный класс.
FileTreeAnalyzer::FileTreeAnalyzer(const QString &root) :
    root_(root),
    sortTree_(GetTreeFilledByRoot())
{
    assert(QDir::isAbsolutePath(root_));

}

size_t FileTreeAnalyzer::GetTotalFilesCount() const
{

}

size_t FileTreeAnalyzer::GetTotalFilesSize() const
{

}

size_t FileTreeAnalyzer::GetAvgFilesSize() const
{

}

size_t FileTreeAnalyzer::GetTotalGroupCount(const QString& groupName) const
{

}

size_t FileTreeAnalyzer::GetTotalGroupSize(const QString& groupName) const
{

}

size_t FileTreeAnalyzer::GetAvgGroupSize(const QString& groupName) const
{

}

size_t FileTreeAnalyzer::GetSubdirsCount()
{

}

FileTreeAnalyzer::fstree_t FileTreeAnalyzer::GetTreeFilledByRoot()
{
    assert(QDir::isAbsolutePath(root_));
    assert(!root_.isEmpty());

    QDir root(root_);

    root.setFilter(QDir::Files);
    QStringList entryList = root.entryList();

    for (auto entry : entryList)
    {
        qDebug() << "entry = " << entry;
    }

    fstree_t result;

    return result;
}























