#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTableView>

#include <progressworker.h>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(IProgressWorker* worker, Object* parent = 0);

private:
    void InitThread();
    void RemoveThread();
    void RiseMsgBox();

signals:
    void operate(const QString& );
    void closeMsgBox();

public slots:

private:
    IProgressWorker* workerClass_;
    QThread workerThread_;
    bool running_;
};


#endif // CONTROLLER_H
