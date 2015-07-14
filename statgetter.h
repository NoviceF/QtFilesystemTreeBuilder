#ifndef STATGETTER_H
#define STATGETTER_H

#include <QObject>
#include <QTableView>
#include <QThread>

class StatGetterThread : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString& parameter);
    void percetnOfWorkDone(const int percent);

    signals:
    void resultReady(const QString &result);
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

public slots:
    void handleResults(const QString& result);
    void setWorkDonePercentage(const int);
};

#endif // STATGETTER_H
