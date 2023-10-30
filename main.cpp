#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include "mainwindow.h"
#include <iostream>

// Подключаем слот к сигналу clicked() кнопки
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
