#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QRgb>
#include <QDirIterator>
#include <QDebug>
#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileFolder("C:/Users/Grega/Dropbox/Faks/3. Letnik/RV/Vaja 1/ct"),
    files()
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    palette = new QRgb[256];
    for(int i = 0; i < 256; i++) {
        palette[i] = QColor::fromRgb(i, i, i).rgb();
      }
    ui->lutSelect->addItem("Izberi paleto...");
    ui->lutSelect->setCurrentIndex(0);
    QDir dir(":/lut");
    foreach(QFileInfo file, dir.entryInfoList()){
       ui->lutSelect->addItem(file.fileName());
      }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectFolderPB_clicked()
{
    QDir dir(QFileDialog::getExistingDirectory(this, tr("Izberi mapo"), fileFolder, QFileDialog::DontResolveSymlinks));
    if(dir.isReadable()){
        fileFolder = dir.absolutePath();
        QFileInfoList fileList = dir.entryInfoList();
        foreach(const QFileInfo file, fileList){
            //std::cout << file.fileName().toStdString() << std::endl;
            if(file.suffix() == "img")
              files.push_back(file.fileName());
          }
        ui->imagesList->addItems(QList<QString>::fromVector(files));
      }
}

void MainWindow::draw_image(const QFileInfo file) {
  QImage image(512, 512, QImage::Format_RGB16);
  std::ifstream inputFile(file.absoluteFilePath().toStdString(), std::ios::binary | std::ios::in);
  if(inputFile.good()) {
      int i = 1;
      signed short pixel;
      while(!inputFile.eof()) {
        inputFile.read((char*)&pixel, 2);
        float colorValue = (((float)pixel + 2048) / 4095) * 255;
        int x = (i + 1) / 512;
        int y = (i + 1) % 512;
        image.setPixel(x, y, palette[(int)colorValue]);
        i++;
        }
    }
  inputFile.close();
  scene->clear();
  scene->addPixmap(QPixmap::fromImage(image));
  ui->imageDIsplay->setScene(scene);
}

void MainWindow::on_lutSelect_currentIndexChanged(const QString &name)
{
    if(name != tr("Izberi paleto...")){
        QFile inputFile(":/lut/" + name);
        if(inputFile.open(QFile::ReadOnly)){
            qDebug() << "File opened.";
            char *c;
            unsigned char r,g,b;
            int i = 0;
            while(!inputFile.atEnd()){
                inputFile.getChar(c);
                r = (*c);
                inputFile.getChar(c);
                g = (*c);
                inputFile.getChar(c);
                b = (*c);
                palette[i] = QColor::fromRgb((int)r, (int)g, (int)b).rgb();
                i++;
                //qDebug() << "R: " << r << ", G: " << g << ", B: " << b;
              }
            //delete c;
            inputFile.close();
            if(currentImage.exists()){
                draw_image(currentImage);
              }
          }
      }
}

void MainWindow::on_imagesList_itemSelectionChanged()
{
  int index = ui->imagesList->currentRow();
  qDebug() << index;
  QFileInfo file(fileFolder + "/" + files.at(index));
  if(file.exists()) {
      currentImage = file;
      draw_image(file);
    }
}
