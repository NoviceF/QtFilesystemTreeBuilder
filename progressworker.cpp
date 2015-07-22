#include "progressworker.h"


IProgressWorker::IProgressWorker(QProgressBar* progBar, QObject* parent) :
    QObject(parent)
{
   connect(this, SIGNAL(setProgressRange(int,int)), progBar, SLOT());
   connect(this, SIGNAL(setProgressValue(int)), progBar, SLOT());
}
