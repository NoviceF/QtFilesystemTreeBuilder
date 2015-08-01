#include <algorithm>
#include <cassert>
#include <memory>


#include <QDebug>
#include <QDesktopWidget>
#include <QProgressBar>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    statGetter_(new StatGetter(this)),
    treeBuilder_(new DirTreeBuilder(this)),
    iconProvider_(new QFileIconProvider()),
    fsTreeModel_(nullptr),
    disks_(FillDisks())
{
    ui_->setupUi(this);

    SetPositionCenter();

    // set connections
    connect(ui_->comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setTreeRootIndex(int)));

    connect(ui_->treeView, SIGNAL(clicked(QModelIndex)), this,
            SLOT(processStatRequest(QModelIndex)));

    // init combobox
    for (const QFileInfo& diskInfo : disks_)
    {
        const QString absolutePath = diskInfo.absoluteFilePath();
        // for linux root
        const QString diskName = absolutePath.size() > 1 ?
                    absolutePath.left(absolutePath.size() - 1) :
                    absolutePath;

        ui_->comboBox->addItem(iconProvider_->icon(diskInfo), diskName,
            absolutePath);
    }

    ui_->comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
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

void MainWindow::setTreeRootIndex(int)
{
    const QString selectedPath = ui_->comboBox->currentData().toString();

    fsTreeModel_ = new SimpleFSModel(this);
    fsTreeModel_->setRootPath(selectedPath);
    ui_->treeView->setModel(fsTreeModel_);
}

void MainWindow::processStatRequest(const QModelIndex& index)
{
    const QString selectedPath = fsTreeModel_->fileInfo(index).absoluteFilePath();
    statGetter_->GetStatsForPath(selectedPath);
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

/*static*/ QVector<QFileInfo> MainWindow::FillDisks()
{
    QVector<QFileInfo> disks;
    const QFileInfoList drives = QDir::drives();
    qCopy(drives.begin(), drives.end(), std::back_inserter(disks));

    return disks;
}

