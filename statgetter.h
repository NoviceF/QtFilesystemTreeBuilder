#ifndef STATGETTER_H
#define STATGETTER_H

#include <QTableView>

#include <controller.h>

class StatGetterThread : public IProgressWorker
{
    Q_OBJECT
public:
    StatGetterThread(const QString& path, QProgressBar* progBar,
        QLabel* label, QObject* parent = 0);

public slots:
    virtual void onStart();
    virtual void onAbort();

private:
    QString path_;
    bool abort_;
};

class StatGetter : public Controller
{
    Q_OBJECT

public:
    explicit StatGetter(QTableView *&tableView, QObject* parent = 0);
    void GetStatsForPath(const QString& rootPath);

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();

private:
    QTableView* tableView_;
    QString pathInWork_;
};

#endif // STATGETTER_H
