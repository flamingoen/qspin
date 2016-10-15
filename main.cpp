#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication::setOrganizationName("QSpinCorp");
    QApplication::setApplicationName("QSpin");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
