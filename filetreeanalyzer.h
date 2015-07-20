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
        const size_t size;
        const size_t count;
    };

public:
    typedef std::map<QString,QVector<QFileInfo>> fstree_t;
    typedef std::map<QString, GroupStats> stattree_t;

    FileTreeAnalyzer(const QString& root);

    size_t GetTotalFilesCount() const;
    size_t GetTotalFilesSize() const;
    size_t GetAvgFilesSize() const;

    size_t GetTotalGroupFilesCount(const QString& groupName) const;
    size_t GetTotalGroupFilesSize(const QString& groupName) const;
    size_t GetAvgGroupFilesSize(const QString& groupName) const;

    size_t GetSubdirsCount();

private:
    stattree_t GetTreeFilledByRoot();

    static size_t GetTotalFilesCount(const fstree_t& tree);
    static size_t GetTotalFilesSize(const fstree_t& tree);

    static size_t GetTotalGroupFilesCount(const QString& groupName,
            const fstree_t& tree);
    static size_t GetTotalGroupFilesSize(const QString& groupName,
            const fstree_t& tree);

private:
    const QString root_;
    const stattree_t sortTree_;

};


#endif // FILETREEANALYZER_H
