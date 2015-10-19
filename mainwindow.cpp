#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QRgb>
#include <QDirIterator>
#include <QDebug>
#include <fstream>
#include <cmath>
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
          if(file.suffix() == "img" || file.suffix() == "cmp")
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
      //      if(!ui->compressPB->isEnabled()){
      //          ui->compressPB->setEnabled(true);
      //        }
      currentImage = file;
      draw_image(file);
    }
}
// kok bitov rabim za stevilko
int BitLen(int nr){
  return round(log2(nr));
}

void MainWindow::on_compressPB_clicked()
{
  char cr[4] = {(char)0, (char)1, (char)2, (char)3};
  qDebug() << "Clicked";
  std::ifstream inputFile(currentImage.absoluteFilePath().toStdString(), std::ios::binary | std::ios::in);
  std::ofstream outputFile(fileFolder.toStdString() + "/" + currentImage.baseName().toStdString() + ".cmp", std::ios::binary | std::ios::out);
  if(inputFile.good()){
      qDebug() << "File read.";
      signed short s;
      signed short prev;
      int nrBits = 0;
      char crByte = 0;
      while(!inputFile.eof()){
          inputFile.read((char*)s, 2);
          if(s == -2048){
              crByte = (crByte << 2) | cr[3]; // zrak
              nrBits += 2;
            } else if(abs(prev - s) > 30){ // absolutno u kurcu
              crByte = (crByte << 2) | cr[2];
              int wBits = 12; // 12 bitov rabim
              short val = (0 << 12) | s;
              while(wBits > 0) {
                  if(nrBits == 8) {
                      outputFile.put(crByte);
                      crByte = 0;
                      nrBits = 0;
                    }
                  int shift = (8 - nrBits);
                  if(wBits < shift)
                    shift = wBits;
                  crByte = crByte << shift;
                  crByte = crByte | (val >> (wBits - shift));
                  nrBits += shift;
                      wBits -= shift;
                }
            } else if(abs(prev - s) <= 30 && abs(prev - s) != 0) { // holy fuck
              int val = prev - s;
              int wBits = BitLen(abs(prev - s)); // min bitov
              if(wBits <= 2) { // [-2, 2]
                val += 2;
                wBits = 2;
              } else if(wBits <= 3) { // [-6, -3][3, 6]
                if(val < 0)
                  val += 6;
                else
                  val = val + 1;
              } else if(wBits <= 4) { // [-14, -7][7, 14]
                if(val < 0)
                  val += 14;
                else
                  val += 1;
              } else { // [-30, -15][15, 30]
                if(val < 0)
                  val += 30;
                else 
                  val += 1;
              }

            } else { // ponovitev
              
            }
          prev = s;
        }
      // write byte
      if(nrBits == 8) {
          outputFile.put(crByte);
          crByte = 0;
          nrBits = 0;
        }
    }
    inputFile.close();
}
