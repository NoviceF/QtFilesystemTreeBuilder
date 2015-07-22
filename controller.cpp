#include <cassert>

#include <QMessageBox>

#include "controller.h"


Controller::Controller(IProgressWorker* worker, QObject *parent):
    QObject(parent),
    workerClass_(worker),
    running_(false)
{
    assert(workerClass_);
}

void Controller::InitThread()
{
//    qDebug() << "init thread";
    running_ = false;

    workerClass_->moveToThread(&workerThread_);

    connect(&workerThread_, SIGNAL(started()), workerClass_, SLOT(onStart()));
//    connect(&workerThread_, &QThread::finished, worker, &QObject::deleteLater);

//    connect(this, &StatGetter::operate, worker, &StatGetterThread::doWork);

//    connect(worker, &StatGetterThread::resultReady, this,
//            &StatGetter::handleResults);
//    connect(worker, &StatGetterThread::percetnOfWorkDone, this,
//            &StatGetter::workDonePercentageHandler);

    workerThread_.start();
    //    qDebug() << "thread start";
}

void Controller::RiseMsgBox(MsgType msgType, const QString& msg)
{
    switch (msgType)
    {
        case MsgType::WorkInProcess:
            RiseWarningMsg(msg);
            break;
        case MsgType::Error:
            RiseErrorMsg(msg);
        default:
            // should never be reached
            break;
    }
}

void Controller::RiseWarningMsg(const QString& msg)
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

void Controller::RiseErrorMsg(const QString& msg)
{

}

void Controller::errorHandler(const QString &errorMsg)
{
    RemoveThread();
    RiseMsgBox(MsgType::Error, "error in thread!");

}
