#ifndef STATGETTER_H
#define STATGETTER_H

#include <QFileInfo>
#include <QTableWidget>

#include <controller.h>

struct GroupStats
{
    const size_t count;
    const size_t size;
};

typedef QVector<QFileInfo> infovec_t;
typedef std::map<QString,infovec_t> fstree_t;
typedef std::map<QString, GroupStats> stattree_t;

struct StatsCont
{
    stattree_t& statTree;
    size_t& subdirsCount;
};



class StatGetterThread : public IProgressWorker
{
    Q_OBJECT

public:
    StatGetterThread(const QString& path, StatsCont& statCont,
        QProgressBar* progBar, QLabel* label, QObject* parent = 0);

private:
    void FillPreAnalysisTree();
    void FillStatTreeByPath();
    void GetSubdirsCount();

    size_t GetTotalGroupFilesCount(const infovec_t& infoList);
    size_t GetTotalGroupFilesSize(const infovec_t& infoList);

public slots:
    virtual void onStart();

private:
    QString path_;
    size_t& subdirsInPathDir_;
    fstree_t preAnalysisTree_;
    stattree_t& statTree_;
};

class StatGetter : public Controller
{
    Q_OBJECT

public:
    explicit StatGetter(QObject* parent = 0);
    void GetStatsForPath(const QString& rootPath);
    void SetView(QTableWidget* view);

    size_t GetTotalFilesCount() const;
    size_t GetTotalFilesSize() const;
    size_t GetAvgSizeAllFiles() const;


private:
    void FillWidgetTable();
    void AddTableRow(int& rowNumber, const QString& name,
                     const size_t* value = nullptr);

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();

private:
    QTableWidget* tableWidget_;
    size_t subdirsInCurPathCount_;
    QString pathInWork_;
    stattree_t statTree_;
};

#endif // STATGETTER_H
