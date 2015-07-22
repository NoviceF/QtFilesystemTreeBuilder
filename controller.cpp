#include <cassert>

#include <QMessageBox>

#include "controller.h"


Controller::Controller(IProgressWorker* worker, QObject *parent):
    QObject(parent),
    workerClass_(worker)
{
    assert(workerClass_);
}

void Controller::InitThread()
{
//    qDebug() << "init thread";
    workerClass_->moveToThread(&workerThread_);

    connect(workerClass_, SIGNAL(error(QString)), this,
            SLOT(onError(QString)));

    connect(&workerThread_, SIGNAL(started()), workerClass_, SLOT(onStart()));

    connect(workerClass_, SIGNAL(finished()), &workerThread_, SLOT(quit()));
    connect(workerClass_, SIGNAL(finished()), workerClass_, SLOT(deleteLater()));
    connect(workerClass_, SIGNAL(finished()), this, SLOT());

    workerThread_.start();
    //    qDebug() << "thread start";
}

void Controller::RemoveThread()
{
    //    qDebug() << "remove thread";
    workerThread_.quit();
//        workerThread_.wait();
}

void Controller::RiseMsgBox(MsgType msgType, const QString& msg)
{
    switch (msgType)
    {
        case MsgType::WorkInProcess:
            RiseWarningMsg();
            break;
        case MsgType::Error:
            RiseErrorMsg(msg);
        default:
            // should never be reached
            break;
    }
}

void Controller::RiseWarningMsg()
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
//        qDebug() << "thread already running";
    QMessageBox msgBox;
    connect(this, SIGNAL(closeMsgBox()), &msgBox, SLOT(close()));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Error occured..");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();

    switch (ret)
    {
        case QMessageBox::Ok:
            // Save was clicked
            break;
        default:
            // should never be reached
            break;
    }
}

void Controller::onError(const QString &errorMsg)
{
    RemoveThread();
    RiseMsgBox(MsgType::Error, errorMsg);

}

void Controller::onWorkDone()
{
    // do work
    RemoveThread();
}
