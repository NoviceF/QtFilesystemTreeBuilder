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

signals:
    finished();
    setProgressRange(int hi, int low);
    setProgressValue(int value);

public slots:
    virtual onStart() = 0;
    virtual onAbort() = 0;
};

#endif // PROGRESSWORKER_H
