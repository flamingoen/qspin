#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "runspin.cpp"
using namespace std;

QString path;
QStatusBar *status;
QTextBrowser *log;
QTextEdit *editor;

QRadioButton *radioSafety;
QRadioButton *radioAcceptance;
QRadioButton *radioLiveness;
QRadioButton *radioColapse;
QRadioButton *radioDH4;

QCheckBox *checkFair;
QCheckBox *checkHSize;

QSpinBox *spinBoxHSize;

FILE *file;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ## Toolbar ##
    QAction *actionLoad = this->findChild<QAction *>("actionLoad");
    QAction *actionSave = this->findChild<QAction *>("actionSave");
    connect(actionLoad, SIGNAL(triggered()) , this,SLOT(loadFile()));
    connect(actionSave, SIGNAL(triggered()) , this,SLOT(saveFile()));

    // ## Verify tab ##
    QPushButton *verifyButton = this->findChild<QPushButton *>("buttonVerify");
    // properties
    radioSafety = this->findChild<QRadioButton *>("radioSafety");
    radioAcceptance = this->findChild<QRadioButton *>("radioAcceptance");
    radioLiveness = this->findChild<QRadioButton *>("radioLiveness");
    checkFair = this->findChild<QCheckBox *>("checkFairness");

    // options
    radioColapse = this->findChild<QRadioButton *>("radioDCOLLAPSE");
    radioDH4 = this->findChild<QRadioButton *>("radioDH4");
    checkHSize = this->findChild<QCheckBox *>("checkHashSize");
    spinBoxHSize = this->findChild<QSpinBox *>("spinBoxHashSize");
    connect(verifyButton, SIGNAL(clicked()) , this,SLOT(runVerify()));

    // ## other ##
    status = this->findChild<QStatusBar *>("statusbar");
    log = this->findChild<QTextBrowser *>("log");
    editor = this->findChild<QTextEdit *>("editor");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFile(){
    path = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Promela Files (*.pml)"));
    if (path!=NULL) {
        char *cpath = new char[path.length()+1];
        status->showMessage("loaded: "+path);
        char buff[512];
        strcpy(cpath , path.toStdString().c_str());
        string str;
        if(!(file = fopen(cpath, "r"))){
            status->showMessage("error in loading file: "+path);
        }
        while(fgets(buff, sizeof(buff), file)!=NULL){
            str += buff;
        }
        editor->setText(QString::fromStdString(str));
        fclose(file);
    }
}

void MainWindow::saveFile() {
    if (path==NULL) {
        path = QFileDialog::getSaveFileName(this, tr("Open File"),"",tr("Promela Files (*.pml)"));
    }
    QString str = editor->toPlainText();
    char *cpath = new char[path.length()+1];
    char *cstr = new char[str.length()+1];
    strcpy(cpath , path.toStdString().c_str());
    strcpy(cstr , str.toStdString().c_str());
    file = fopen (cpath,"w");
    if (file!=NULL)
    {
        fputs (cstr,file);
        status->showMessage("Saved file to: "+path);
        fclose (file);
    }
}

void MainWindow::runSimulate() {
    //    QString out;
    //    out = QString::fromStdString(runSim(file.toStdString()));
    //    log->setText(out);
}

void MainWindow::runVerify(){
    log->clear();
    saveFile();
    QString out;
    status->showMessage("Validation: running");
    out = QString::fromStdString(runVer(path.toStdString(),getRunOptions(),getCompileOptions()));
    log->setText(out);
    status->showMessage("Validation: finished");
}

string MainWindow::getRunOptions() {
    string out = "";

    if (checkFair->isChecked()) {
        if (radioSafety->isChecked()) {
            status->showMessage("WARNING: Fairness only applicable for acceptance and liveness properties");
        }
        else                             out+="-f ";
    }
    if (radioAcceptance->isChecked())    out+="-a ";
    else if (radioLiveness->isChecked()) out+="-l ";

    if (checkHSize->isChecked()) {
        stringstream ss;
        ss << "-w" << spinBoxHSize->value() << " ";
        out += ss.str();
    }

    return out;
}

string MainWindow::getCompileOptions() {
    string out = "";
    if (radioColapse->isChecked())           out +="-DCOLLAPSE ";
        else if (radioDH4->isChecked())      out +="-DH4 ";
    if (radioSafety->isChecked())            out +="-DSAFTY ";
        else if (radioLiveness->isChecked()) out +="-DNP ";
    return out;
}
