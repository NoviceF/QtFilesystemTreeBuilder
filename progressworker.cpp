#include "progressworker.h"


IProgressWorker::IProgressWorker(QProgressBar* progBar, QLabel* label,
         QObject* parent) :
    QObject(parent),
    progBar_(progBar),
    label_(label)
{
   connect(this, SIGNAL(setProgressRange(int,int)), progBar_,
           SLOT(setRange(int,int)));
   connect(this, SIGNAL(setProgressValue(int)), progBar_, SLOT(setValue(int)));
   connect(this, SIGNAL(showProgressBar()), progBar_, SLOT(show()));
   connect(this, SIGNAL(hideProgressBar()), progBar_, SLOT(hide()));

   connect(this, SIGNAL(setLabel(QString)), label_, SLOT(setText(QString)));
   connect(this, SIGNAL(showLabel()), label_, SLOT(show()));
   connect(this, SIGNAL(hideLabel()), label_, SLOT(hide()));
}

IProgressWorker::~IProgressWorker()
{
   disconnect(this, SIGNAL(setProgressRange(int,int)), progBar_,
           SLOT(setRange(int,int)));
   disconnect(this, SIGNAL(setProgressValue(int)), progBar_, SLOT(setValue(int)));
   disconnect(this, SIGNAL(showProgressBar()), progBar_, SLOT(show()));
   disconnect(this, SIGNAL(hideProgressBar()), progBar_, SLOT(hide()));

   disconnect(this, SIGNAL(setLabel(QString)), label_, SLOT(setText(QString)));
   disconnect(this, SIGNAL(showLabel()), label_, SLOT(show()));
   disconnect(this, SIGNAL(hideLabel()), label_, SLOT(hide()));

}
