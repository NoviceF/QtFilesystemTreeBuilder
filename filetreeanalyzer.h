#ifndef FILETREEANALYZER_H
#define FILETREEANALYZER_H

#include <QDir>
#include <QMap>
#include <QString>
#include <QVector>

class FileTreeAnalyzer
{
public:
    typedef QMap<QString,QVector<QString>> fstree_t;

    FileTreeAnalyzer(const QString& root);

    size_t GetTotalFilesCount() const;
    size_t GetTotalFilesSize() const;
    size_t GetAvgFilesSize() const;

    size_t GetTotalGroupCount(const QString& groupName) const;
    size_t GetTotalGroupSize(const QString& groupName) const;
    size_t GetAvgGroupSize(const QString& groupName) const;

    size_t GetSubdirsCount();

private:
    fstree_t GetTreeFilledByRoot();

private:
    const QString root_;
    const fstree_t sortTree_;
};


#endif // FILETREEANALYZER_H
