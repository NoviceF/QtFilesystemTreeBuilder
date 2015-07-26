#ifndef STATGETTER_H
#define STATGETTER_H

#include <QFileInfo>
#include <QTableWidget>

#include <controller.h>

class StatGetterThread : public IProgressWorker
{
    Q_OBJECT

    struct GroupStats
    {
        const size_t count;
        const size_t size;
    };

public:
    typedef QVector<QFileInfo> infovec_t;
    typedef std::map<QString,infovec_t> fstree_t;
    typedef std::map<QString, GroupStats> stattree_t;

public:
    StatGetterThread(const QString& path, QProgressBar* progBar,
        QLabel* label, QTableWidget* statTable, QObject* parent = 0);

    size_t GetTotalFilesCount() const;
    size_t GetTotalFilesSize() const;
    size_t GetAvgSizeAllFiles() const;

    size_t GetTotalGroupFilesCount(const QString& groupName) const;
    size_t GetTotalGroupFilesSize(const QString& groupName) const;
    size_t GetAvgGroupFilesSize(const QString& groupName) const;

    size_t GetSubdirsCount();

private:
    stattree_t FillStatTreeByPath();

    size_t GetTotalFilesCount();
    size_t GetTotalFilesSize();

    static size_t GetTotalGroupFilesCount(const infovec_t& infoList);
    static size_t GetTotalGroupFilesSize(const infovec_t& infoList);

public slots:
    virtual void onStart();
    virtual void onAbort();

private:
    QString path_;
    bool abort_;
    stattree_t statTree_;
    QTableWidget* statTable_;
};

class StatGetter : public Controller
{
    Q_OBJECT

public:
    explicit StatGetter(QObject* parent = 0);
    void GetStatsForPath(const QString& rootPath);
    void SetView(QTableWidget* view);
    QTableWidget* GetView();

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();

private:
    QTableWidget* tableView_;
    QString pathInWork_;
};

#endif // STATGETTER_H
