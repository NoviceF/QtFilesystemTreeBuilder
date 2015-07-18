#include <cassert>

#include <QDebug>
#include <QDesktopWidget>
#include <QProgressBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    fsModel_(new QFileSystemModel(this)),
    statGetter_(new StatGetter(ui_->tableView, this))
{
    ui_->setupUi(this);

    SetPositionCenter();

    connect(ui_->comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setTreeRootIndex(int)));

//    QString selectedPath("d:\\tmp");
//    QString selectedPath("");
    const QString selectedPath("/home/novice/proj/cpp/dirtest");

    fsModel_->setRootPath(selectedPath);
    fsModel_->setFilter(QDir::Drives);
    fsModel_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    ui_->comboBox->setModel(fsModel_);
    ui_->comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui_->comboBox->setRootModelIndex(fsModel_->index(fsModel_->rootPath()));
    ui_->comboBox->setCurrentIndex(0);

    ui_->treeView->setModel(fsModel_);

    QProgressBar* progBar = new QProgressBar;

    progBar->setMinimum(0);
    progBar->setMaximum(100);
    progBar->setValue(0);
    progBar->setAlignment(Qt::AlignRight);

    statusBar()->addWidget(progBar, 1);
    progBar->hide();

    connect(statGetter_, &StatGetter::workDoneStatus, progBar,
            &QProgressBar::setValue);

	connect(ui_->treeView, SIGNAL(clicked(QModelIndex)), this,
            SLOT(processStatRequest(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::setTreeRootIndex(int index)
{
    assert(ui_->comboBox->currentIndex() == index);

    const QString itemName = ui_->comboBox->currentText();
    const QString selectedPath(fsModel_->rootPath() + "/" + itemName);
    //TODO: проверить под виндой
    QDir::toNativeSeparators(selectedPath);
    QDir selectedDir(selectedPath);

    QModelIndex fsIndex = fsModel_->index(selectedDir.absolutePath());

    if(fsModel_->canFetchMore(fsIndex))
    // make sure the entries in the dir are loaded
    fsModel_->fetchMore(fsIndex);

    ui_->treeView->setRootIndex(fsIndex);
    ui_->treeView->setCurrentIndex(fsIndex);
}

void MainWindow::processStatRequest(QModelIndex index)
{

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

//void MainWindow::on_treeView_clicked(const QModelIndex& index)
//{
//    qDebug() << "on_treeView_clicked";
//    const QString selectedPath = fsModel_->fileInfo(index).absoluteFilePath();
//    statGetter_->GetStatsForPath(selectedPath);
//}
