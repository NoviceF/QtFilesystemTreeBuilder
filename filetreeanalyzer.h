#ifndef FILETREEANALYZER_H
#define FILETREEANALYZER_H

#include <map>

#include <QDir>
#include <QString>
#include <QVector>

class FileTreeAnalyzer
{
    struct GroupStats
    {
        const size_t count;
        const size_t size;
    };

public:
    typedef QVector<QFileInfo> infovec_t;
    typedef std::map<QString,infovec_t> fstree_t;
    typedef std::map<QString, GroupStats> stattree_t;

    FileTreeAnalyzer(const QString& root);

    size_t GetTotalFilesCount() const;
    size_t GetTotalFilesSize() const;
    size_t GetAvgSizeAllFiles() const;

    size_t GetTotalGroupFilesCount(const QString& groupName) const;
    size_t GetTotalGroupFilesSize(const QString& groupName) const;
    size_t GetAvgGroupFilesSize(const QString& groupName) const;

    size_t GetSubdirsCount();

private:
    stattree_t GetTreeFilledByRoot();

    size_t GetTotalFilesCount();
    size_t GetTotalFilesSize();

    static size_t GetTotalGroupFilesCount(const QString& groupName,
            const infovec_t& infoList);
    static size_t GetTotalGroupFilesSize(const QString& groupName,
            const infovec_t& infoList);

private:
    const QString root_;
    stattree_t statTree_;
};


#endif // FILETREEANALYZER_H
