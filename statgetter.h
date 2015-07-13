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

signals:
    void operate(const QString& );

public slots:
    void handleResults(const QString& result);
};

//class Controller : public QObject
//{
//    Q_OBJECT
//    QThread workerThread;
//public:
//    Controller() {
//        StatGetterThread *worker = new StatGetterThread;
//        worker->moveToThread(&workerThread);
//        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
//        connect(this, &Controller::operate, worker, &StatGetterThread::doWork);
//        connect(worker, &StatGetterThread::resultReady, this, &Controller::handleResults);
//        workerThread.start();
//    }
//    ~Controller() {
//        workerThread.quit();
//        workerThread.wait();
//    }
//public slots:
//    void handleResults(const QString &);
//signals:
//    void operate(const QString &);
//};

#endif // STATGETTER_H
