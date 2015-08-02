#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileIconProvider>

#include "dirtreebuilder.h"
#include "statgetter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void setTreeRootIndex(int);
    void processStatRequest(const QModelIndex& index);

private:
    void SetPositionCenter();
    static QVector<QFileInfo> GetDisks();

private:
    Ui::MainWindow* ui_;
    StatGetter* statGetter_;
    DirTreeBuilder* treeBuilder_;
    const QFileIconProvider* iconProvider_;
    const QVector<QFileInfo> disks_;
};

#endif // MAINWINDOW_H
