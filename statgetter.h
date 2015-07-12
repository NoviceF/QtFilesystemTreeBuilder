#ifndef STATGETTER_H
#define STATGETTER_H

#include <QObject>
#include <QTableView>

class StatGetterThread : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString& parameter) {
        QString result;
        /* ... here is the expensive or blocking operation ... */
        emit resultReady(result);
    }

signals:
    void resultReady(const QString &result);
};

class StatGetter : public QObject
{
    Q_OBJECT
public:
    explicit StatGetter(QTableView *&tableView, QObject* parent = 0);
    ~StatGetter();

private:
    void GetStatsForPath(const QString& rootPath);
    void InitThread();
    void RemoveThread();

private:
    QTableView* tableView_;
    QThread workerThread_;
    bool running_;

signals:
    void operate(const QString& );

public slots:
    void handleResults(const QString& );
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
