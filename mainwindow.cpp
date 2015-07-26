#include <cassert>
#include <unistd.h>

#include <QDebug>
#include <QDesktopWidget>
#include <QProgressBar>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    fsComboModel_(new QFileSystemModel(this)),
    fsTreeModel_(new QFileSystemModel(this)),
    statGetter_(new StatGetter(this)),
    treeBuilder_(new DirTreeBuilder(this))

{
    ui_->setupUi(this);

    SetPositionCenter();

    connect(ui_->comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setTreeRootIndex(int)));

    connect(ui_->treeView, SIGNAL(clicked(QModelIndex)), this,
            SLOT(processStatRequest(QModelIndex)));

//     const QString selectedPath("");
    const QString selectedPath("/home/novice/proj/cpp/dirtest");

    ui_->comboBox->blockSignals(true);
//    fsComboModel_->setFilter(QDir::Drives);
    fsComboModel_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    fsComboModel_->setRootPath(selectedPath);

    // принимаем сигналы только после загрузки ui
    QTimer::singleShot(0, this, SLOT(unblockCombo()));

    fsTreeModel_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
//    fsTreeModel_->setRootPath(selectedPath);

//    ui_->treeView->setModel(fsTreeModel_);

    QProgressBar* progBar = new QProgressBar;

    progBar->setAlignment(Qt::AlignRight);

    QLabel* label = new QLabel;
    label->setAlignment(Qt::AlignCenter);

    statusBar()->addWidget(label, 1);
    statusBar()->addWidget(progBar, 1);

    progBar->hide();

    treeBuilder_->SetProgBar(progBar);
    treeBuilder_->SetLabel(label);

    statGetter_->SetProgBar(progBar);
    statGetter_->SetLabel(label);
    statGetter_->SetView(ui_->tableWidget);

}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::setTreeRootIndex(int index)
{
    assert(ui_->comboBox->currentIndex() == index);

    const QString itemName = ui_->comboBox->currentText();
    QString selectedPath;

    if (fsComboModel_->rootPath().isEmpty())
        selectedPath = itemName + "/";
    else
        selectedPath = fsComboModel_->rootPath() + "/" + itemName;

    selectedPath = QDir::toNativeSeparators(selectedPath);

    if (ui_->treeView->model() == nullptr)
    {
        initTreeRoot(selectedPath);
//        return;
    }

    //TODO: проверить под виндой
    QDir selectedDir(selectedPath);

    QModelIndex fsIndex = fsTreeModel_->index(selectedDir.absolutePath());

    if (fsTreeModel_->canFetchMore(fsIndex))
    {
        // make sure the entries in the dir are loaded
        fsTreeModel_->fetchMore(fsIndex);
    }

    ui_->treeView->setRootIndex(fsIndex);
    ui_->treeView->setCurrentIndex(fsIndex);
}

void MainWindow::processStatRequest(const QModelIndex& index)
{
//    qDebug() << "processStatRequest";
    const QString selectedPath = fsTreeModel_->fileInfo(index).absoluteFilePath();
    statGetter_->GetStatsForPath(selectedPath);
}

void MainWindow::initTreeRoot(const QString& path)
{
    ui_->treeView->setModel(fsTreeModel_);
    fsTreeModel_->setRootPath(path);

//    treeBuilder_->BuildDirTree(path);
}

void MainWindow::unblockCombo()
{
    ui_->comboBox->setModel(fsComboModel_);
    ui_->comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui_->comboBox->setRootModelIndex(fsComboModel_->index(fsComboModel_->rootPath()));

    ui_->comboBox->blockSignals(false);
}

void MainWindow::SetPositionCenter()
{
    if (this->isFullScreen())
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

