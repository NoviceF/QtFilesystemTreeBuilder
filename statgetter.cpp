#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QWidget>

#include <unistd.h>

#include "statgetter.h"
#include <filetreeanalyzer.h>

StatGetterThread::StatGetterThread() :
    percentOfWorkDone_(0)
{

}

void StatGetterThread::doWork(const QString& parameter)
{
    QString result (parameter);
//    qDebug() << "do calculation in thread";

//    FileTreeAnalyzer analyzer(parameter);

    for (int i = 0; i < 5; ++i)
    {
        percentOfWorkDone_ = percentOfWorkDone_ + 20;
        emit percetnOfWorkDone(percentOfWorkDone_);
        sleep(2);
    }

    emit resultReady(result);
    emit percetnOfWorkDone(percentOfWorkDone_);
    percentOfWorkDone_ = -1;
}

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
    if (!running_)
    {
        pathInWork_ = rootPath;
        InitThread();
        running_ = true;
//        qDebug() << "send job to thread";
        emit operate(rootPath);
    }
    else if (rootPath != pathInWork_)
    {
        RiseMsgBox();
    }
}

void StatGetter::InitThread()
{
//    qDebug() << "init thread";
    running_ = false;

    StatGetterThread* worker = new StatGetterThread;
    worker->moveToThread(&workerThread_);

    connect(&workerThread_, &QThread::finished, worker, &QObject::deleteLater);

    connect(this, &StatGetter::operate, worker, &StatGetterThread::doWork);

    connect(worker, &StatGetterThread::resultReady, this,
            &StatGetter::handleResults);
    connect(worker, &StatGetterThread::percetnOfWorkDone, this,
            &StatGetter::workDonePercentageHandler);

    workerThread_.start();
//    qDebug() << "thread start";
}

void StatGetter::RemoveThread()
{
//    qDebug() << "remove thread";
    running_ = false;
    workerThread_.quit();
    //    workerThread_.wait();
}

void StatGetter::RiseMsgBox()
{
//        qDebug() << "thread already running";
    QMessageBox msgBox;
    connect(this, SIGNAL(closeMsgBox()), &msgBox, SLOT(close()));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Previous operation in progress now..");
    msgBox.setInformativeText("Do you want to interrupt operation?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
            // Save was clicked
            break;
        case QMessageBox::No:
            // Don't Save was clicked
            break;
        default:
            // should never be reached
            break;
    }
}

void StatGetter::handleResults(const QString& result)
{
//    qDebug() << "get thread result" << result;
    RemoveThread();
}

void StatGetter::workDonePercentageHandler(int percent)
{
    const QString msg("filesystem tree in building process..");

    if (percent < 0 || percent > 100)
    {
        RemoveThread();
        emit(workDoneStatus(-1, msg));
    }
    else
    {
        if (percent == 100)
        {
            emit closeMsgBox();
            emit workDoneStatus(-1, msg);
        }
        else
        {
            emit(workDoneStatus(percent, msg));
        }
    }
}


