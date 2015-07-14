#include <QDebug>
#include <QString>
//#include <QMessageBox>
#include <QProgressDialog>
#include <QDialog>
#include <QApplication>
#include <QWidget>

#include <unistd.h>

#include "statgetter.h"

StatGetter::StatGetter(QTableView*& tableView, QObject* parent) :
    QObject(parent),
    tableView_(tableView),
    running_(false),
    percentOfWorkDone_(0)
{

}

StatGetter::~StatGetter()
{

}

void StatGetter::GetStatsForPath(const QString& rootPath)
{
    QString path = rootPath;

    if (!running_)
    {
        InitThread();
        running_ = true;
        qDebug() << "send job to thread";
        emit operate(rootPath);
    }
    else
    {
        qDebug() << "thread already running";
        // диалог
        int numFiles = 5;
        QProgressDialog progress("The treatment is the previous query...",
                                 "Cancel", 0, 100,
                                 qobject_cast<QWidget *>(parent()));

        progress.setWindowModality(Qt::WindowModal);

        for (int i = 0; i < numFiles; i++) {

            progress.setValue(i);

            if (progress.wasCanceled())
                break;
            //... copy one file
        }

        progress.setValue(numFiles);
    }
}

void StatGetter::InitThread()
{
    qDebug() << "init thread";
    percentOfWorkDone_ = 0;
    running_ = false;
    StatGetterThread* worker = new StatGetterThread;
    worker->moveToThread(&workerThread_);
    connect(&workerThread_, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &StatGetter::operate, worker, &StatGetterThread::doWork);
    connect(worker, &StatGetterThread::resultReady, this,
            &StatGetter::handleResults);
    //TODO: привязать к прогресс бару основного окна
    connect(worker, &StatGetterThread::percetnOfWorkDone, this,
            &StatGetter::setWorkDonePercentage);
    workerThread_.start();
    qDebug() << "thread start";
}

void StatGetter::RemoveThread()
{
    qDebug() << "remove thread";
    running_ = false;
    workerThread_.quit();
    workerThread_.wait();
}

void StatGetter::handleResults(const QString& result)
{
    qDebug() << "get thread result" << result;
    RemoveThread();
}

void StatGetterThread::doWork(const QString& parameter)
{
    QString result (parameter);
    qDebug() << "do calculation in thread";
    sleep(3);

    emit resultReady(result);
}
