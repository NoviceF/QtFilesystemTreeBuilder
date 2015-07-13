#include <QDebug>
#include <QString>
#include <QMessageBox>

#include <unistd.h>

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
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("Previous operation in progress now..");
        msgBox.setInformativeText("Do you want to interrupt operation?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No |
                                  QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::Yes:
                // Save was clicked
                break;
            case QMessageBox::No:
                // Don't Save was clicked
                break;
            case QMessageBox::Cancel:
                // Cancel was clicked
                break;
            default:
                // should never be reached
                break;
        }
    }
}

void StatGetter::InitThread()
{
    qDebug() << "init thread";
    StatGetterThread* worker = new StatGetterThread;
    worker->moveToThread(&workerThread_);
    connect(&workerThread_, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &StatGetter::operate, worker, &StatGetterThread::doWork);
    connect(worker, &StatGetterThread::resultReady, this, &StatGetter::handleResults);
    workerThread_.start();
    qDebug() << "thread start";
    running_ = false;
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
