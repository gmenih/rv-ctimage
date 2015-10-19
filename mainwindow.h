#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QListWidgetItem>

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

  void on_lutSelect_currentIndexChanged(const QString &arg1);

  void on_imagesList_itemSelectionChanged();

  void on_compressPB_clicked();

private:
    Ui::MainWindow *ui;
    QString fileFolder;
    QVector<QString> files;
    QRgb* palette;
    QFileInfo currentImage;
    QGraphicsScene* scene;

    void draw_image(const QFileInfo file);
};

#endif // MAINWINDOW_H
