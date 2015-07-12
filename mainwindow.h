#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QString>

#include <statgetter.h>

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
    void on_comboBox_activated(const QString &arg2);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_treeView_clicked(const QModelIndex &rootPath);

private:
    void SetPositionCenter();

private:
    Ui::MainWindow* ui_;
    QFileSystemModel* dirModel_;
    QFileSystemModel* fileModel_;
    StatGetter* statGetter_;
};

#endif // MAINWINDOW_H
