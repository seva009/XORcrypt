#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include "mainwindow.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
