#include "mainwindow.h"
#include <QCoreApplication>
#include "./ui_mainwindow.h"
#include "xor/dFile.h"
#include <iostream>

std::string path;
std::string strPass;
int threads;

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
    crypt.init(path, strPass); //50
    ui->cryptProgress->setValue(40);
    crypt.cryptFile(); //73
    ui->cryptProgress->setValue(60);
    crypt.saveFile(); //90
    ui->cryptProgress->setValue(80);
    crypt.wipe();
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
