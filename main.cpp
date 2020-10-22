#include "mainwindow.h"
#include "Files/download.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    qRegisterMetaType<download>("download");
    qRegisterMetaType<progressdialog>("progressdialog");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
