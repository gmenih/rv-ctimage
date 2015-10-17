#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
  void on_selectFolderPB_clicked();

private:
    Ui::MainWindow *ui;
    QString fileFolder;
    QVector<QString> files;
};

#endif // MAINWINDOW_H
