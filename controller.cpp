#include <cassert>

#include <QMessageBox>

#include "controller.h"


Controller::Controller(QObject* parent):
    QObject(parent),
    running_(false)
{
}

Controller::~Controller()
{
    if (running_)
        RemoveThread();
}

void Controller::RunThread(IProgressWorker* worker)
{
    assert(!running_);

    if (running_)
        return;

    worker->moveToThread(&workerThread_);

    connect(worker, SIGNAL(finished()), this, SLOT(onWorkDone()));

    workerThread_.start();
    running_ = true;
}

void Controller::RemoveThread()
{
    workerThread_.quit();
    workerThread_.wait();
    running_ = false;
}

void Controller::RiseRunningThreadWarningMsg()
{
    QMessageBox msgBox;
    connect(this, SIGNAL(closeMsgBox()), &msgBox, SLOT(close()));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Previous operation in progress now..");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    msgBox.exec();
}

void Controller::SetProgBar(QProgressBar* progBar)
{
    progBar_ = progBar;
    assert(progBar_);
}

QProgressBar* Controller::GetProgBar()
{
    if (!progBar_)
    {
        throw std::runtime_error("Controler::GetProgBar: Progress bar "
             "must be set before use.");
    }

    return progBar_;
}

void Controller::SetLabel(QLabel* label)
{
    label_ = label;
    assert(label_);
}

QLabel* Controller::GetLabel()
{
    if (!label_)
    {
        throw std::runtime_error("Controller::SetLabel: Label "
             "must be set before use.");
    }

    return label_;
}

const QThread& Controller::GetWorkerThread()
{
    return workerThread_;
}

void Controller::RiseErrorMsg(const QString& msg)
{
    QMessageBox msgBox;
    connect(this, SIGNAL(closeMsgBox()), &msgBox, SLOT(close()));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Error occured..");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void Controller::onWorkDone()
{
    RemoveThread();
    emit closeMsgBox();
}
