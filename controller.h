#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTableView>
#include <QThread>

#include <progressworker.h>

class Controller : public QObject
{
    Q_OBJECT

    enum class MsgType
    {
        WorkInProcess = 0,
        Error
    };

public:
    explicit Controller(IProgressWorker* worker, QObject* parent = 0);

protected:
    ~Controller() = default;

private:
    void InitThread();
    void RemoveThread();
    void RiseMsgBox(MsgType msgType, const QString& msg = QString());
    void RiseWarningMsg();
    void RiseErrorMsg(const QString& msg);

signals:
    void abort();
    void closeMsgBox();

public slots:
    virtual void onError(const QString& errorMsg);
    virtual void onWorkDone();

private:
    IProgressWorker* workerClass_;
    QThread workerThread_;
    bool running_;
};




#endif // CONTROLLER_H
