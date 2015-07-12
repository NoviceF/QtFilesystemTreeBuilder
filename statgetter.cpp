#include <QString>

#include "statgetter.h"

StatGetter::StatGetter(QTableView*& tableView, QObject* parent) :
    QObject(parent),
    tableView_(tableView),
    running_(false)
{

}

StatGetter::~StatGetter()
{

}

void StatGetter::GetStatsForPath(const QString& rootPath)
{
    QString path = rootPath;


    workerThread_.start();
}

void StatGetter::InitThread()
{
    StatGetterThread* worker = new StatGetterThread;
    worker->moveToThread(&workerThread_);
    connect(&workerThread_, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::operate, worker, &StatGetterThread::doWork);
    connect(worker, &statgetterthread::resultready, this, &controller::handleresults);
    running_ = false;
}

void StatGetter::RemoveThread()
{
    workerThread_.quit();
    workerThread_.wait();
}

void StatGetter::handleResults(const QString&)
{
    running_ = false;
}

