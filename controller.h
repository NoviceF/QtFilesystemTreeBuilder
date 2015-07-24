#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTableView>
#include <QThread>

#include <progressworker.h>

class Controller : public QObject
{
    Q_OBJECT

public:
    bool IsRunning() { return running_; }
    void RunThread(IProgressWorker* worker);
    bool RiseRunningThreadWarningMsg();
    void SetProgBar(QProgressBar* progBar);
    QProgressBar* GetProgBar();
    void SetLabel(QLabel* label);
    QLabel* GetLabel();

protected:
    explicit Controller(QObject* parent = 0);
    ~Controller() = default;

private:
    void RemoveThread();
    void RiseErrorMsg(const QString& msg);

signals:
    void abort();
    void closeMsgBox();

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();

private:
    QProgressBar* progBar_;
    QLabel* label_;
    QThread workerThread_;
    bool running_;
};




#endif // CONTROLLER_H
