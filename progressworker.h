#ifndef PROGRESSWORKER_H
#define PROGRESSWORKER_H

#include <QObject>
#include <QProgressBar>

// Интерфейса потока, информирующего о состоянии выполнения задачи
class IProgressWorker : public QObject
{
    Q_OBJECT

public:
    IProgressWorker(QProgressBar* progBar, QObject* parent);
    virtual ~IProgressWorker(){}

signals:
    void finished();
    void error(const QString& errorMsg);
    void setProgressRange(int hi, int low);
    void setProgressValue(int value);

public slots:
    virtual void onStart() = 0;
    virtual void onAbort() = 0;
};

#endif // PROGRESSWORKER_H
