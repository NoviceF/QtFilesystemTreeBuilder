#include <cassert>

#include "controller.h"


Controller::Controller(IProgressWorker* worker, Object* parent):
    workerClass_(worker),
    QObject(parent),
    running_(false)
{
    assert(workerClass_);
}

void Controller::InitThread()
{
//    qDebug() << "init thread";
    running_ = false;

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
