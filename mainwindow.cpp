#include <cassert>
#include <unistd.h>

#include <QDebug>
#include <QDesktopWidget>
#include <QProgressBar>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    fsComboModel_(new QFileSystemModel(this)),
    fsTreeModel_(nullptr),
    statGetter_(new StatGetter(this)),
    treeBuilder_(new DirTreeBuilder(this))
{
    ui_->setupUi(this);

    SetPositionCenter();

    // set connections
    connect(ui_->comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setTreeRootIndex(int)));

    connect(ui_->treeView, SIGNAL(clicked(QModelIndex)), this,
            SLOT(processStatRequest(QModelIndex)));

    // init combobox
//     const QString selectedPath("");
    const QString selectedPath("/home/novice/proj/cpp/dirtest");
    fsComboModel_->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

//    fsComboModel_->setFilter(QDir::Drives);
    fsComboModel_->setRootPath(selectedPath);

    ui_->comboBox->blockSignals(true);
    ui_->comboBox->setModel(fsComboModel_);
    ui_->comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui_->comboBox->setRootModelIndex(fsComboModel_->index(fsComboModel_->rootPath()));
//    const int textIndex = ui_->comboBox->findData("C:/", Qt::DisplayRole);
//    ui_->comboBox->setCurrentIndex(textIndex);
    ui_->comboBox->blockSignals(false);
    ui_->comboBox->setCurrentIndex(0);

    // init statusbar
    QProgressBar* progBar = new QProgressBar;
    progBar->setAlignment(Qt::AlignRight);

    QLabel* label = new QLabel;
    label->setAlignment(Qt::AlignCenter);

    statusBar()->addWidget(label, 1);
    statusBar()->addWidget(progBar, 1);

    progBar->hide();

    // init table widget
    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // init worker class
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
    const int row = index;
    const int col = 0;
    const QModelIndex rootIndex = fsComboModel_->index(fsComboModel_->rootPath());
    const QModelIndex modelIndex = rootIndex.child(row, col);
    const QString selectedPath =
            fsComboModel_->fileInfo(modelIndex).absoluteFilePath();

    SimpleFSModel* oldModel = fsTreeModel_;

    fsTreeModel_ = new SimpleFSModel(this);
    fsTreeModel_->setRootPath(selectedPath);
    ui_->treeView->setModel(fsTreeModel_);

    delete oldModel;
}

void MainWindow::processStatRequest(const QModelIndex& index)
{
//    const QString selectedPath = fsTreeModel_->fileInfo(index).absoluteFilePath();
//    statGetter_->GetStatsForPath(selectedPath);
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

