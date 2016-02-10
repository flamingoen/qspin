#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "runspin.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(simulateButton,SIGNAL(clicked()),this,SLOT(runSimulate()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::runSimulate(string file){
    exec("spin -u200 -p "+file);
}
