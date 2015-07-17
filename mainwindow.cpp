#include <QDebug>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    fsModel_(new QFileSystemModel(this)),
    statGetter_(new StatGetter(ui_->tableView, this))
{
    SetPositionCenter();

    ui_->setupUi(this);

//    QFileSystemModel* model = new QFileSystemModel(this);

//    model->setFilter(QDir::Drives);
//    model->setRootPath("");
    fsModel_->setRootPath("/home/novice/proj/cpp/dirtest");
    fsModel_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    ui_->comboBox->setModel(fsModel_);
    ui_->comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui_->comboBox->setRootModelIndex(fsModel_->index("/home/novice/proj/cpp/dirtest"));
    ui_->comboBox->setCurrentIndex(0);
//    ui_->comboBox->activated("");

//    dirModel_->setRootPath("c:\\");
    ui_->treeView->setModel(fsModel_);
//    connect(ui_->comboBox, &QComboBox::activated, this, &MainWindow::setTreeRootIndex);
    connect(ui_->comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setTreeRootIndex(int)));

    ui_->progressBar->setMinimum(0);
    ui_->progressBar->setMaximum(100);
    ui_->progressBar->setValue(0);

    connect(statGetter_, &StatGetter::workDoneStatus, ui_->progressBar,
            &QProgressBar::setValue);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::setTreeRootIndex(int index)
{
    const QString itemName = ui_->comboBox->currentText();
    QString selectedDirName("/home/novice/proj/cpp/dirtest");
    selectedDirName = selectedDirName + "/" + itemName;
    QDir selectedDir(selectedDirName);
//    QModelIndex modIndex = fsModel_->index(itemName, 2);
//    QModelIndex modIndex = fsModel_->index(QDir::currentPath());
    QModelIndex modIndex = fsModel_->index(selectedDir.absolutePath());
    ui_->treeView->setRootIndex(modIndex);

//    if(fsModel_->canFetchMore(index))
//    // make sure the entries in the dir are loaded
//    fsModel_->fetchMore(index);
////    ui_->treeView->sel




//    qDebug() << "index " << index << "=" << fsModel_->filePath(index);

//    ui_->comboBox->

//    const int cbIndex = ui_->comboBox->currentIndex();
//    const QString itemName = ui_->comboBox->itemData(cbIndex).toString();
//    const int itemIntName = ui_->comboBox->itemData(cbIndex).toInt();
    qDebug() << "itemName = " << itemName ;
//    ui_->treeView->setRootIndex(fsModel_->index(index));

//    ui->comboBox->setCurrentIndex(0);
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

//    ui_->treeView->setRootIndex(fsModel_->index(QDir::absoluteFilePath(arg1)));


//void MainWindow::on_treeView_clicked(const QModelIndex& index)
//{
//    qDebug() << "on_treeView_clicked";
//    const QString selectedPath = fsModel_->fileInfo(index).absoluteFilePath();
//    statGetter_->GetStatsForPath(selectedPath);
//}
