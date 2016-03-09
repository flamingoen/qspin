#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "runspin.cpp"
using namespace std;

QString path;
QString filename;
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
QSpinBox *spinBoxSteps;
QComboBox *comboChoice;

runProcess process;

QFile file;

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
    connect(verifyButton, SIGNAL(clicked()) , this, SLOT(runVerify()));

    // ## Simulation tab
    QPushButton *buttonRandomSim = this->findChild<QPushButton *>("buttonRandomSim");
    QPushButton *buttonInteractiveSim = this->findChild<QPushButton *>("buttonInteractiveSim");
    QPushButton *buttonSubmit = this->findChild<QPushButton *>("buttonSubmit");
    QPushButton *buttonGuidedSim = this->findChild<QPushButton *>("buttonGuidedSim");
    comboChoice = this->findChild<QComboBox *>("comboChoice");
    spinBoxSteps = this->findChild<QSpinBox *>("spinBoxSteps");
    connect(buttonRandomSim, SIGNAL(clicked()), this, SLOT(runRandomSimulation()));
    connect(buttonInteractiveSim, SIGNAL(clicked()), this, SLOT(runInteractiveSimulation()));
    connect(buttonSubmit, SIGNAL(clicked()),this,SLOT(runSubmitInteractiveSimulation()));
    connect(buttonGuidedSim, SIGNAL(clicked()),this,SLOT(runGuidedSimulation()));

    // options
    radioColapse = this->findChild<QRadioButton *>("radioDCOLLAPSE");
    radioDH4 = this->findChild<QRadioButton *>("radioDH4");
    checkHSize = this->findChild<QCheckBox *>("checkHashSize");
    spinBoxHSize = this->findChild<QSpinBox *>("spinBoxHashSize");

    // ## other ##
    status = this->findChild<QStatusBar *>("statusbar");
    log = this->findChild<QTextBrowser *>("log");
    editor = this->findChild<QTextEdit *>("editor");
}

MainWindow::~MainWindow() {
    process.terminate();
    process.runAndWait("rm",QStringList()<<filename+".trail");
    delete ui;
}

void MainWindow::loadFile() {
    process.terminate();
    if (filename!=NULL) process.runAndWait("rm",QStringList()<<filename+".trail");

    path = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Promela Files (*.pml)"));
    if (path!=NULL) {
        editor->clear();
        QFile file(path);

        QRegExp rx("/((([a-z]|[A-Z]|\\d)+).pml)");
        rx.indexIn(path);
        filename = rx.cap(1);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        status->showMessage("Failed to open "+path);
        } else {
            QTextStream in(&file);
            while(!in.atEnd()) {
                editor->append(in.readLine());
            }
            file.close();
            status->showMessage("File loaded: "+path);
        }
    } else {
        status->showMessage("No file choosen");
    }
}

void MainWindow::saveFile() {
    if (path==NULL) {
        path = QFileDialog::getSaveFileName(this, tr("Open File"),"",tr("Promela Files (*.pml)"));
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        status->showMessage("Could not save "+path);
    } else {
        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();
        status->showMessage("File saved: "+path);
    }
}

void MainWindow::runRandomSimulation() {
    process.terminate();
    log->clear();
    saveFile();
    QString out = process.runGetOutput("spin",QStringList() << "-u200" << "-p" << "-g" << "-l" << path);
    log->append(out);
}

void MainWindow::runInteractiveSimulation(){
    process.terminate();
    log->clear();
    saveFile();
    log->append(process.runGetOutputWaitForInput("spin",QStringList() << "-i" << path));
}

void MainWindow::runGuidedSimulation(){
    process.terminate();
    log->clear();
    QString trailPath = path + ".trail";
    process.runAndWait("cp" , QStringList() << filename+".trail" << trailPath);
    log->append(process.runGetOutput("spin",QStringList() << "-t" << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "-u250" << path));
    process.runAndWait("rm",QStringList() << trailPath);
}

void MainWindow::runSubmitInteractiveSimulation() {
    if (!process.finished()) {
        log->append(process.runInputGetOutput(comboChoice->currentText()));
    } else { process.terminate(); }
}

void MainWindow::runVerify(){
    process.terminate();
    log->clear();
    saveFile();
    process.runAndWait("spin",QStringList()<<"-a" << path);
    process.runAndWait("cc",getCompileOptions() << "pan.c");
    QString out = process.runGetOutput("./pan",getRunOptions());
    log->append(out);
    status->showMessage("Validation: finished");
}

QStringList MainWindow::getRunOptions() {
    QStringList out;

    if (checkFair->isChecked()) {
        if (radioSafety->isChecked()) {
            log->append("WARNING: Fairness only applicable for acceptance and liveness properties (ignored)");
        }
        else                             out << "-f ";
    }
    if (radioAcceptance->isChecked())    out << "-a ";
    else if (radioLiveness->isChecked()) out << "-l ";

    if (checkHSize->isChecked()) {
        stringstream ss;
        ss << "-w" << spinBoxHSize->value() << " ";
        out << QString::fromStdString(ss.str());
    }
    return out;
}

QStringList MainWindow::getCompileOptions() {
    QStringList out;
    if (radioColapse->isChecked())           out << "-DCOLLAPSE ";
    else if (radioDH4->isChecked())          out << "-DH4 ";
    if (radioSafety->isChecked())            out <<"-DSAFTY ";
    else if (radioLiveness->isChecked())     out <<"-DNP ";
    return out << "-o" << "pan";
}
