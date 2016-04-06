#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "verificationoutput.h"
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
QCheckBox *checkOptDepth;

QSpinBox *spinBoxHSize;
QSpinBox *spinBoxSteps;
QSpinBox *spinBoxSDepth;
QComboBox *comboChoice;

QProcess *process;
VerificationRun* verification;

QFile file;


// Verification tab
QLabel *spinVerLabel;
QLabel *evalLabel;
QLabel *partialLabel;
QLabel *neverLabel;
QLabel *assertionLabel;
QLabel *acceptanceLabel;
QLabel *invalidLabel;

QLabel * errorLabel;
QLabel * depthLabel;
QLabel * storedstatesLabel;
QLabel * matchedstatesLabel;
QLabel * transitionLabel;
QLabel * atomicLabel;
QLabel * statesizeLabel;
QLabel * hashconflictsLabel;
QLabel * hashsizeLabel;

VerificationOutput *verificationOutput;


bool newVerification = false;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("QSpin");

    // ## Verification Tab ##
    verificationOutput = new VerificationOutput();

    //Setting font of groupbox titles
    QGroupBox *statespaceprop = this->findChild<QGroupBox *>("groupBox_4");
    statespaceprop->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } "); // The stylesheet is not inherited to children of the QGroupBox,
                                                                                                    // thereby the labels won't suffer the effect of the change in the parents stylesheet.
    QGroupBox *statespacespecs = this->findChild<QGroupBox *>("groupBox_5");
    statespacespecs->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } "); // The stylesheet is not inherited to children of the QGroupBox,
                                                                                                     // thereby the labels won't suffer the effect of the change in the parents stylesheet
                                                                                                     // Connecting to objects

    //Statespaceprop groupbox
    spinVerLabel = this->findChild<QLabel *>("spinVerLabel");
    evalLabel = this->findChild<QLabel *>("evalLabel");
    partialLabel = this->findChild<QLabel *>("partialLabel");
    neverLabel = this->findChild<QLabel *>("neverLabel");
    assertionLabel = this->findChild<QLabel *>("assertionLabel");
    acceptanceLabel = this->findChild<QLabel *>("acceptanceLabel");
    invalidLabel = this->findChild<QLabel *>("invalidLabel");

    // Statespacespec groupbox
    errorLabel = this->findChild<QLabel *>("errorLabel");
    depthLabel = this->findChild<QLabel *>("depthLabel");
    storedstatesLabel = this->findChild<QLabel *>("storedstatesLabel");
    matchedstatesLabel = this->findChild<QLabel *>("matchedstatesLabel");
    transitionLabel = this->findChild<QLabel *>("transitionLabel");
    atomicLabel = this->findChild<QLabel *>("atomicLabel");
    statesizeLabel = this->findChild<QLabel *>("statesizeLabel");
    hashconflictsLabel = this->findChild<QLabel *>("hashconflictsLabel");
    hashsizeLabel = this->findChild<QLabel *>("hashsizeLabel");

    // ## Toolbar ##
    QAction *actionLoad = this->findChild<QAction *>("actionLoad");
    QAction *actionSave = this->findChild<QAction *>("actionSave");
    QAction *actionAbort = this->findChild<QAction *>("actionAbort");
    QAction *actionCheckSyntax = this->findChild<QAction *>("actionCheck_syntax");
    connect(actionLoad, SIGNAL(triggered()) , this,SLOT(loadFile()));
    connect(actionSave, SIGNAL(triggered()) , this,SLOT(saveFile()));
    connect(actionAbort, SIGNAL(triggered()),this,SLOT(terminateProcess()));
    connect(actionCheckSyntax, SIGNAL(triggered()), this , SLOT(checkSyntax()));

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
    spinBoxSDepth = this->findChild<QSpinBox *>("spinBoxSearchDepth");
    checkOptDepth = this->findChild<QCheckBox *>("checkOptimizeDepth");

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
        status->showMessage("No file chosen");
    }
}

void MainWindow::saveFile() {
    if (path==NULL) {
        path = QFileDialog::getSaveFileName(this, tr("Save File"),"",tr("Promela Files (*.pml)"));
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

void MainWindow::checkSyntax() {
    prepareRun();
    process->disconnect();
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(checkSyntaxErrorHighlight()));
    process->start(SPIN,QStringList() << "-a" << path.replace(" ","\\ "));
}

void MainWindow::checkSyntaxErrorHighlight() {
    QString str = process->readAllStandardOutput();
    QRegExp rxLine("spin: "+path+":(\\d+)");
    QRegExp rxError("spin: "+path+":\\d+, Error:(\\D*)");
    QStringList lineNoList;
    QStringList errorList;
    int pos = 0;
    while ((pos = rxError.indexIn(str , (rxLine.indexIn(str, pos)) )) != -1) {
        QString lineNo = rxLine.cap(1);
        QString error = rxError.cap(1);
        lineNoList << lineNo;
        errorList << error;
        outputLog->append("Error at line "+lineNo+": "+error);
        pos += rxLine.matchedLength()+rxError.matchedLength();
    }
    editor->HighlightErrorLines(lineNoList);
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
    process->start(SPIN,QStringList() << "-t" << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "-u250" << path.replace(" ","\\ "));
}

// BUG: Hvis man spammer verify knappen bliver gamle threads ikke lukker (memory leak)
void MainWindow::runVerify(){
    fileCleanup();
    if (prepareRun()) {
        outputLog->clear();
        verification = new VerificationRun(path, verificationType(),checkFair->isChecked(),getCompileOptions(),searchDepth(),hashSize());
        QThread* thread = new QThread();
        verification->moveToThread(thread);
        connect(thread,SIGNAL(started()),verification,SLOT(start()));
        connect(verification, SIGNAL(readReady()),this,SLOT(verificationReadReady()));
        connect(verification, SIGNAL(finished()),this,SLOT(verificationFinished()));
        connect(verification, SIGNAL(finished()), thread, SLOT(quit()));
        connect(verification, SIGNAL(statusChanged()),this,SLOT(verificationStatusChange()));
        connect(thread, SIGNAL(finished()),thread,SLOT(deleteLater()));
        thread->start();
    }
}

VerificationRun::VerificationType MainWindow::verificationType() {
    if (radioAcceptance->isChecked())
        return VerificationRun::Acceptance;
    if (radioLiveness->isChecked())
        return VerificationRun::Liveness;
    else return VerificationRun::Safety;
}

int MainWindow::searchDepth() {
    if (spinBoxSDepth->value()!=10000)
        return spinBoxSDepth->value();
    else return -1;
}

int MainWindow::hashSize() {
    if (checkHSize->isChecked())
        return spinBoxHSize->value();
    else return -1;
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
    QString output = process->readAllStandardOutput();
    outputLog->append(output);
    status->showMessage("Finished");
}

void MainWindow::verificationFinished() {
    outputLog->append(verification->readOutput());
    processVerificationOutput(outputLog->toPlainText());
    if (checkOptDepth->isChecked()) {
        spinBoxSDepth->setValue((verificationOutput->depth).toInt()+10);
    }
    status->showMessage("Finished");
}

void MainWindow::runProcessReadReady() {
    outputLog->append(process->readAllStandardOutput());
}

void MainWindow::verificationReadReady() {
    outputLog->append(verification->readOutput());
}

void MainWindow::verificationStatusChange() {
    status->showMessage(verification->readStatus());
}

void MainWindow::resetProcess() {
    process = new QProcess();
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(runProcessReadReady()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(runProcessFinished()));
}

// returns if there is a file to run or not
bool MainWindow::prepareRun(bool clearLog){
    resetProcess();
    if (path!=NULL) saveFile();
    else loadFile();
    if (clearLog) outputLog->clear();
    return path!=NULL;
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

void MainWindow::processVerificationOutput(QString output){

    verificationOutput->processVerification(output);

    updateVerificationTab();

}

void MainWindow::updateVerificationTab(){
    // SPINVERSIONLABEL
    spinVerLabel->setText(verificationOutput->spinVer);
    // EVALUATIONLABEL
    evalLabel->setStyleSheet("background-color: " + verificationOutput->eval+ ";");

    // STATESPACE PROP
    // PARTIAL ORDER REDUCTION LABEL
    partialLabel->setText("Partial order reduction: " + verificationOutput->partial);
    // NEVER CLAIM LABEL
    neverLabel->setText("Never claim: "+ verificationOutput->never);
    // ASSERTION LABEL
    assertionLabel->setText("Assertion violations: "+ verificationOutput->assertion);
    //ACCEPTANCE LABEL
    acceptanceLabel->setText(verificationOutput->acceptanceType+" cycles: " + verificationOutput->acceptance);
    //INVALID END STATES LABEL
    invalidLabel->setText("Invalid end states: " + verificationOutput->invalid);

    // STATESPACE SPECS
    errorLabel->setText("Errors: " + verificationOutput->errors);
    depthLabel->setText("Depth reached: " + verificationOutput->depth);
    storedstatesLabel->setText("Stored states: " + verificationOutput->storedStates);
    matchedstatesLabel->setText("Matched states: " + verificationOutput->matchedStates);
    transitionLabel->setText("Transitions taken: " + verificationOutput->transitions);
    atomicLabel->setText("Atomic steps: " + verificationOutput->atomic);
    statesizeLabel->setText("State size (bytes): " + verificationOutput->statesize);
    hashconflictsLabel->setText("Hash conflicts: " + verificationOutput->hashconflict);
    hashsizeLabel->setText("Hash size: " + verificationOutput->hashsize);

}
