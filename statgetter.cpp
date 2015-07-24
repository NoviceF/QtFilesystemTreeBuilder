#include <cassert>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QWidget>

#include <unistd.h>

#include "statgetter.h"
#include <filetreeanalyzer.h>


StatGetterThread::StatGetterThread(const QString& path, QProgressBar* progBar,
    QLabel* label, QObject* parent) :
    IProgressWorker(progBar, label, parent),
    path_(path),
    abort_(false)
{
    if (path_.isEmpty())
        throw std::runtime_error("Path was not setted.");
}

void StatGetterThread::onStart()
{

}

void StatGetterThread::onAbort()
{
    //TODO: проверять
    if (!abort_)
        abort_ = true;

    emit error("Abort called");
}

///
/// \brief StatGetter
///
StatGetter::StatGetter(QTableView*& tableView, QObject* parent) :
    Controller(parent),
    tableView_(tableView)
{
    assert(tableView_);
}

void StatGetter::GetStatsForPath(const QString& rootPath)
{
    pathInWork_ = rootPath;
    assert(!pathInWork_.isEmpty());

    if (IsRunning())
    {
        if (RiseRunningThreadWarningMsg())
           emit abort();
        else
            return;
    }

    StatGetterThread* statGetterThread =
            new StatGetterThread(rootPath, GetProgBar(), GetLabel());

    RunThread(statGetterThread);
}

void StatGetter::onError(const QString& errorMsg)
{
    Controller::onError(errorMsg);
}

void StatGetter::onWorkDone()
{
    // do work
    Controller::onWorkDone();
}

