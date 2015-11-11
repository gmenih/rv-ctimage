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
#include <QMessageBox>
#include <bitset>
#include <iostream>
#include "BitWriter.h"
#include "BitReader.h"

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
  files.clear();
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
      int i = 0;
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
          char *c;
          unsigned char r,g,b;
          int i = 0;
          while(i < inputFile.bytesAvailable()){
              inputFile.getChar(c);
              r = *c;
              inputFile.getChar(c);
              g = *c;
              inputFile.getChar(c);
              b = *c;
              palette[i / 3] = QColor::fromRgb((int)r, (int)g, (int)b).rgb();
              i+=3;
            }
          inputFile.close();
          if(currentImage.suffix() == "img"){
              draw_image(currentImage);
            } else if(currentImage.suffix() == "cmp"){
              decompressImage(currentImage);
            }
        }
    }
}
void MainWindow::on_imagesList_itemSelectionChanged()
{
  int index = ui->imagesList->currentRow();
  qDebug() << index;
  QFileInfo file(fileFolder + "/" + files.at(index));
  if(file.exists() && file.suffix() == "img") {
      if(!ui->compressPB->isEnabled()){
          ui->compressPB->setEnabled(true);
        }
      currentImage = file;
      draw_image(file);
    } else if (file.exists() && file.suffix() == "cmp") {
      if(ui->compressPB->isEnabled()){
          ui->compressPB->setEnabled(false);
        }
      currentImage = file;
      decompressImage(file);
    }
}
void MainWindow::compressFile(QFileInfo file) {
  char DIFF = 0, REPEAT = 1, ABSC = 2, AIR = 3;
  std::ifstream inFile(file.absoluteFilePath().toStdString(), std::ios::binary | std::ios::in);
  std::ofstream outFile((fileFolder + "/" + file.baseName() + ".cmp").toStdString(), std::ios::binary | std::ios::out);
  // vars
  BitWriter bw(outFile);
  signed short readByte, prev = -9999;
  //char writeByte = 0, nBits = 0;
  char repeats = 0;
  while(!inFile.eof()) {
      inFile.read((char *)&readByte, 2); // preberem 2 byta v short
      if(repeats > 0 && prev != readByte || repeats >= 63){
          bw.writeBits(REPEAT, 2);
          bw.writeBits(repeats, 6);
          repeats = 0;
        }
      if(readByte == -2048) {
          bw.writeBits(AIR, 2);
        } else if(abs(prev - readByte) == 0) {
          repeats++;
        } else if(abs(prev - readByte) < 30) {
          int diff = readByte - prev;
          int absDiff = abs(diff);
          int bLen = 0;
          if(absDiff <= 2) {
              if(diff < 0)
                diff += 2;
              else
                diff += 1;
            } else if(absDiff <= 6) {
              if(diff < 0)
                diff += 6;
              else
                diff += 1;
              bLen = 1;
            } else if(absDiff <= 14) {
              if(diff < 0)
                diff += 14;
              else
                diff += 1;
              bLen = 2;
            } else if(absDiff < 30) {
              if(diff < 0)
                diff += 30;
              else
                diff += 1;
              bLen = 3;
            }
          bw.writeBits(DIFF, 2);
          bw.writeBits(bLen, 2);
          bw.writeBits(diff, (bLen + 2));
        } else {
          bw.writeBits(ABSC, 2);
          bw.writeBits((int)(readByte + 2048), 12);
        }
      prev = readByte;
    }
  if(repeats > 0) {
      bw.writeBits(REPEAT, 2);
      bw.writeBits(repeats - 1, 6);
    }
  bw.writeOffset();
  inFile.close();
  outFile.close();
}

void MainWindow::decompressImage(QFileInfo file) {
  char DIFF = 0, REPEAT = 1, ABSC = 2, AIR = 3;
  QImage image(512, 512, QImage::Format_RGB16);
  std::ifstream inFile(file.absoluteFilePath().toStdString(), std::ios::binary | std::ios::in);
  //ofstream outFile("original0185.img", ios::binary | ios::out);
  BitReader br(inFile);
  int pixel = 0, prev, nrBytes = 0;
  while(!inFile.eof()){
      if(nrBytes >= (512 * 512))
        break;
      int action = br.readBits(2);
      if(action == AIR) {
          pixel = -2048;
        } else if(action == REPEAT) {
          int repeats = br.readBits(6);
          float pxVal = (((float)prev + 2048) / 4095) * 255;
          for(int i = 0; i < repeats - 1; i++){
              int x = (nrBytes + 1) / 512;
              int y = (nrBytes + 1) % 512;
              image.setPixel(x, y, palette[(int)pxVal]);
              nrBytes++;
            }
        } else if(action == DIFF) {
          int we = br.readBits(2);
          we+= 2;
          int a = br.readBits(we);
          int e = pow(2, we);
          if(a < e / 2)
            a -= (e - 2);
          else
            a -= 1;
          pixel = prev + a;

        } else if(action == ABSC) {
          pixel = br.readBits((int)12);
          pixel = (pixel - 2048);
        }
      prev = pixel;
      int x = (nrBytes + 1) / 512;
      int y = (nrBytes + 1) % 512;
      float pxVal = (((float)pixel + 2048) / 4095) * 255;
      image.setPixel(x, y, palette[(int)pxVal]);
      nrBytes++;

    }
  inFile.close();
  scene->clear();
  scene->addPixmap(QPixmap::fromImage(image));
  ui->imageDIsplay->setScene(scene);
}

void MainWindow::on_compressPB_clicked()
{
  compressFile(currentImage);
  QMessageBox msgBox;
  QFileInfo newFile(currentImage.absolutePath() + "/" + currentImage.baseName() + ".cmp");
  if(newFile.exists()){
      float razmerje = (float)currentImage.size() / (float)newFile.size();
      msgBox.setText("Datoteka je bila stisnjena. Razmerje stiskanja: " + QString::number(razmerje));
    } else {
      msgBox.setText("Napaka pri kompresiji.");
    }
  msgBox.exec();
  QDir dir(fileFolder);
  files.clear();
  if(dir.isReadable()){
      fileFolder = dir.absolutePath();
      QFileInfoList fileList = dir.entryInfoList();
      foreach(const QFileInfo file, fileList){
          std::cout << file.fileName().toStdString() << std::endl;
          if(file.suffix() == "img" || file.suffix() == "cmp")
            files.push_back(file.fileName());
        }
      ui->imagesList->clear();
      ui->imagesList->addItems(QList<QString>::fromVector(files));
    }
}
