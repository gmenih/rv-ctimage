#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QListWidgetItem>
#include <QStringListModel>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    files()
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectFolderPB_clicked()
{
    QDir dir(QFileDialog::getExistingDirectory(this, tr("Izberi mapo"), "C:", QFileDialog::DontResolveSymlinks));
    if(dir.isReadable()){
        QFileInfoList fileList = dir.entryInfoList();
        foreach(const QFileInfo file, fileList){
            //std::cout << file.fileName().toStdString() << std::endl;
            if(file.suffix() == "img")
              files.push_back(file.fileName());
          }
        ui->imagesList->setModel(new QStringListModel(QList<QString>::fromVector(files)));
      }
}
