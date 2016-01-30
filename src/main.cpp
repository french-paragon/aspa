#include "mainwindow.h"
#include <QApplication>

#include <ctime>

int main(int argc, char *argv[])
{

	std::srand ( unsigned ( std::time(0) ) ); //seed rand

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
