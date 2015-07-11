#include <QDesktopWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    dirModel_(new QFileSystemModel(this)),
    fileModel_(new QFileSystemModel(this))
{
    SetPositionCenter();

    ui_->setupUi(this);

    QFileSystemModel* model = new QFileSystemModel(this);
    model->setFilter(QDir::Drives);
    //model->setFilter(QDir::AllEntries);
    //model->setRootPath("/home/novice");
    ui_->comboBox->setModel(model);

    //dirModel_->setRootPath(initPath);
    dirModel_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    ui_->treeView->setModel(dirModel_);


}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::SetPositionCenter()
{
    if(this->isFullScreen())
        return;

    const QSize size = this->size();

    QDesktopWidget* desktop = QApplication::desktop();
    const int screenWidth = desktop->width();   // returns screen width
    const int screenHeight = desktop->height();  // returns screen height
    const int mainWidth = size.width();
    const int mainHeight = size.height();
    const int widthPos = (screenWidth/2) - (mainWidth/2);
    const int heightPos = (screenHeight/2) - (mainHeight/2);

    this->move(widthPos,heightPos);
}


void MainWindow::on_comboBox_activated(const QString& arg1)
{
    dirModel_->setRootPath(arg1);
}

void MainWindow::on_comboBox_currentIndexChanged(const QString& arg1)
{
    dirModel_->setRootPath(arg1);
}
