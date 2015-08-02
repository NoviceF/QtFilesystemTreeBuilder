#include "progressworker.h"


IProgressWorker::IProgressWorker(QProgressBar* progBar, QLabel* label,
         QObject* parent) :
    QObject(parent)
{
   connect(this, SIGNAL(setProgressRange(int,int)), progBar,
           SLOT(setRange(int,int)));
   connect(this, SIGNAL(setProgressValue(int)), progBar, SLOT(setValue(int)));
   connect(this, SIGNAL(showProgressBar()), progBar, SLOT(show()));
   connect(this, SIGNAL(hideProgressBar()), progBar, SLOT(hide()));

   connect(this, SIGNAL(setLabel(QString)), label, SLOT(setText(QString)));
   connect(this, SIGNAL(showLabel()), label, SLOT(show()));
   connect(this, SIGNAL(hideLabel()), label, SLOT(hide()));
}
