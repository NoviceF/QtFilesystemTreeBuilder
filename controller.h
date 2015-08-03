#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>

#include <progressworker.h>

class Controller : public QObject
{
    Q_OBJECT

public:
    bool IsRunning() const { return running_; }
    void RunThread(IProgressWorker* worker);
    void RemoveThread();
    void RiseRunningThreadWarningMsg() const;
    void SetProgBar(QProgressBar* progBar);
    QProgressBar* GetProgBar();
    void SetLabel(QLabel* label);
    QLabel* GetLabel();
    const QThread& GetWorkerThread();

protected:
    explicit Controller(QObject* parent = 0);
    ~Controller();

private:
    void RiseErrorMsg(const QString& msg) const;

signals:
    void closeMsgBox();

public slots:
    virtual void onWorkDone();

private:
    QProgressBar* progBar_;
    QLabel* label_;
    QThread workerThread_;
    bool running_;
};




#endif // CONTROLLER_H
