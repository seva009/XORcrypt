#include "mainwindow.h"
#include <QCoreApplication>
#include "./ui_mainwindow.h"
#include "xor/dFile.h"
#include <iostream>

std::string path;
std::string strPass;
std::string ndPass = "noPass";
int threads;
int clc = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_xorStart_clicked()
{
    strPass = ui->password->text().toStdString();
    path = ui->lineEdit->text().toStdString();
    threads = ui->threads->value();
    Crypt crypt;
    ui->cryptProgress->setValue(0);
    crypt.setThreads(threads); //10
    ui->cryptProgress->setValue(20);
    if (clc % 2) {
        ndPass = ui->ndpassword->text().toStdString();
        crypt.init(path, strPass, ndPass); //50
    } else {
        crypt.init(path, strPass); //50
    }
    ui->cryptProgress->setValue(40);
    crypt.cryptFile(); //73
    ui->cryptProgress->setValue(60);
    crypt.wipe();
    ui->cryptProgress->setValue(80);
    crypt.saveFile(); //90
    crypt.fileWipe();
    ui->cryptProgress->setValue(100);
}

void MainWindow::on_threads_valueChanged(int arg1)
{
    if (ui->threads->value() > 1) {
        ui->thName->setText("threads");
    } else {
        ui->thName->setText("thread");
    }
}

void MainWindow::on_hasNd_stateChanged(int arg1)
{
    clc++;
}
