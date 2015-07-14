#ifndef STATGETTER_H
#define STATGETTER_H

#include <QObject>
#include <QTableView>
#include <QThread>

class StatGetterThread : public QObject
{
    Q_OBJECT
public:
    StatGetterThread();

private:
    int percentOfWorkDone_;


public slots:
    void doWork(const QString& parameter);

    signals:
    void resultReady(const QString &result);
    void percetnOfWorkDone(int percent);
};

class StatGetter : public QObject
{
    Q_OBJECT

public:
    explicit StatGetter(QTableView *&tableView, QObject* parent = 0);
    ~StatGetter();
    void GetStatsForPath(const QString& rootPath);

private:
    void InitThread();
    void RemoveThread();

private:
    QTableView* tableView_;
    QThread workerThread_;
    bool running_;
    int percentOfWorkDone_;

signals:
    void operate(const QString& );
    void workDoneStatus(int);

public slots:
    void handleResults(const QString& result);
    void workDonePercentageHandler(int percent);
};

#endif // STATGETTER_H
