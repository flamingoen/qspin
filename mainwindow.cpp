#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "runspin.cpp"
using namespace std;

QString file;
QTextBrowser *log;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPushButton *simulateButton = this->findChild<QPushButton *>("simulateButton");
    QPushButton *verifyButton = this->findChild<QPushButton *>("verifyButton");
    QPushButton *loadButton = this->findChild<QPushButton *>("loadButton");
    log = this->findChild<QTextBrowser *>("log");
    connect(simulateButton, SIGNAL(clicked()) , this,SLOT(runSimulate()));
    connect(verifyButton, SIGNAL(clicked()) , this,SLOT(runVerify()));
    connect(loadButton, SIGNAL(clicked()) , this,SLOT(loadFile()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFile(){
   file = QFileDialog::getOpenFileName(this, tr("Open File"),"");
   log->setText("loaded: "+file);
}

void MainWindow::runSimulate() {
    QString out;
    out = QString::fromStdString(runSim(file.toStdString()));
    log->setText(out);
}

void MainWindow::runVerify(){
    QString out;
    out = QString::fromStdString(runVer(file.toStdString()));
    log->setText(out);
}
