#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;


#ifdef _WIN32
    #define SPIN  "spin\\spin.exe"
    #define CCOMPILER "gcc"
#else
    #define SPIN "spin"
    #define CCOMPILER "cc"
#endif


QString path;
QString filename;
QStatusBar *status;
CodeEditor *editor;
QTextEdit *outputLog;

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

QProcess *process;

QFile file;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("QSpin");

    // ## Toolbar ##
    QAction *actionLoad = this->findChild<QAction *>("actionLoad");
    QAction *actionSave = this->findChild<QAction *>("actionSave");
    QAction *actionAbort = this->findChild<QAction *>("actionAbort");
    connect(actionLoad, SIGNAL(triggered()) , this,SLOT(loadFile()));
    connect(actionSave, SIGNAL(triggered()) , this,SLOT(saveFile()));
    connect(actionAbort, SIGNAL(triggered()),this,SLOT(terminateProcess()));

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
    outputLog = this->findChild<QTextEdit *>("log");
    editor = (CodeEditor*) this->findChild<QPlainTextEdit *>("editor");

    new Highlighter(editor->document());
}

MainWindow::~MainWindow() {
    fileCleanup();
    delete ui;
}

void MainWindow::loadFile() {
    // TODO: Indsæt temporær filepath, når man åbner dialogen overskrives filepath til "" hvis ikke man vælger en ny fil.
    if (filename!=NULL) fileCleanup();
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
                editor->appendPlainText(in.readLine());
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
    prepareRun();
    process->start(SPIN,QStringList() << "-u200" << "-p" << "-g" << "-l" << path);
}

void MainWindow::runInteractiveSimulation() {
    prepareRun();
    process->start(SPIN,QStringList() << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "-i"  << path);
}

void MainWindow::runSubmitInteractiveSimulation() {
    if (!process->state() == 0) {
        QString cmd = comboChoice->currentText() + "\n";
        process->write(cmd.toLatin1().data());
    } else { process->terminate(); }
}
//TODO: Burde være delt op i signaler
void MainWindow::runGuidedSimulation(){
    prepareRun(); // TODO: Will try to save file if a file is not loaded and this button is pushed
    QString trailPath = QDir::toNativeSeparators(path) + ".trail";
    QFile::copy(filename+".trail", trailPath);
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(fileCleanup()));
    process->start(SPIN,QStringList() << "-t" << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "-u250" << path);
}

void MainWindow::runVerify(){
    prepareRun();
    fileCleanup();
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(runCompile()));
    status->showMessage("verification: Making model");
    process->start(SPIN,QStringList()<<"-a" << path);
}

void MainWindow::runCompile(){
    // BUG: If you change properties after clicking run verify it fucks up.
    prepareRun();
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(runPan()));
    if (process->state()==QProcess::NotRunning)
        status->showMessage("verification: Compiling pan.c");
        process->start(CCOMPILER,getCompileOptions() << "pan.c");
}

void MainWindow::runPan(){
    prepareRun();
    if (process->state()==QProcess::NotRunning)
        status->showMessage("verification: Running verification");
        process->start("./pan",getRunOptions());
}

QStringList MainWindow::getRunOptions() {
    QStringList out;

    if (checkFair->isChecked()) {
        if (radioSafety->isChecked()) {
            outputLog->append("WARNING: Fairness only applicable for acceptance and liveness properties (ignored)");
        }
        else                                out << "-f ";
    }
    if (radioAcceptance->isChecked())       out << "-a ";
    else if (radioLiveness->isChecked())    out << "-l ";

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

void MainWindow::runProcessFinished() {
    outputLog->append(process->readAllStandardOutput());
    status->showMessage("Finished");
}



void MainWindow::resetProcess() {
    process = new QProcess();
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(runProcessFinished()));
}


void MainWindow::prepareRun(bool clearLog){
    resetProcess();
    if (path==NULL) loadFile();
    saveFile();
    if (clearLog) outputLog->clear();
}

void MainWindow::terminateProcess(){
    if(process != NULL){
        process->disconnect();
        outputLog->clear();
        status->showMessage("Process killed.");
        process->kill();
    }

}

void MainWindow::fileCleanup(){
    QDir dir(QDir::currentPath());
    dir.setNameFilters(QStringList() << "pan*" << "*.trail");
    dir.setFilter(QDir::Files);
    dir.remove(path+".trail");
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}
