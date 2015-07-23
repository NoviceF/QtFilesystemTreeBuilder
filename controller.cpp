﻿#include <cassert>

#include <QMessageBox>

#include "controller.h"


Controller::Controller(QObject* parent):
    QObject(parent)
{
}

void Controller::RunThread(IProgressWorker* worker)
{
//    qDebug() << "init thread";
    if (running_)
        RemoveThread();

    worker->moveToThread(&workerThread_);

    connect(worker, SIGNAL(error(QString)), this, SLOT(onError(QString)));

    connect(&workerThread_, SIGNAL(started()), worker, SLOT(onStart()));

    connect(worker, SIGNAL(finished()), &workerThread_, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));

    workerThread_.start();
    running_ = true;
    //    qDebug() << "thread start";
}

void Controller::RemoveThread()
{
    //    qDebug() << "remove thread";
    workerThread_.quit();
    running_ = false;
//        workerThread_.wait();
}

bool Controller::RiseRunningThreadWarningMsg()
{
//        qDebug() << "thread already running";
    QMessageBox msgBox;
    connect(this, SIGNAL(closeMsgBox()), &msgBox, SLOT(close()));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Previous operation in progress now..");
    msgBox.setInformativeText("Do you want to interrupt operation?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    int ret = msgBox.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
            // interrupt
            return true;
            break;
        case QMessageBox::No:
            // Don't interrutp
            return false;
            break;
        default:
            assert(false);
            return false;
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
    RiseErrorMsg(errorMsg);

}

void Controller::onWorkDone()
{
    // do work
    RemoveThread();
}
