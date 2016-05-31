#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("QSpin");

    // ## Verification Tab ##
    verificationOutput = new VerificationOutput();

    //Setting font of groupbox titles
    QGroupBox *statespaceprop = this->findChild<QGroupBox *>("groupBox_4");
    statespaceprop->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } "); // The stylesheet is not inherited to children of the QGroupBox, thereby the labels won't suffer the effect of the change in the parents stylesheet.
    QGroupBox *statespacespecs = this->findChild<QGroupBox *>("groupBox_5");
    statespacespecs->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } "); // The stylesheet is not inherited to children of the QGroupBox, thereby the labels won't suffer the effect of the change in the parents stylesheet.
    QGroupBox *memoryusage = this->findChild<QGroupBox *>("groupBox_6");
    memoryusage->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } ");    // The stylesheet is not inherited to children of the QGroupBox thereby the labels won't suffer the effect of the change in the parents stylesheet.

    // Connecting to objects

    //Statespaceprop groupbox
    spinVerLabel = this->findChild<QLabel *>("spinVerLabel");
    evalLabel = this->findChild<QLabel *>("evalLabel");
    partialLabel = this->findChild<QLabel *>("partialLabel_2");
    neverLabel = this->findChild<QLabel *>("neverLabel_2");
    assertionLabel = this->findChild<QLabel *>("assertionLabel_2");
    acceptanceLabel = this->findChild<QLabel *>("acceptanceLabel_2");
    invalidLabel = this->findChild<QLabel *>("invalidLabel_2");

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

    // Memory usage groupbox
    statememoryLabel = this->findChild<QLabel *>("statememoryLabel");
    hashmemoryLabel = this->findChild<QLabel *>("hashmemoryLabel");
    DFSmemoryLabel = this->findChild<QLabel *>("DFSmemoryLabel");
    totalmemoryLabel = this->findChild<QLabel *>("totalmemoryLabel");

    timestampLabel = this->findChild<QLabel *>("timestampLabel");
    ltlList = this->findChild<QListWidget *>("ltlList");

    // ## Toolbar ##
    QAction *actionLoad = this->findChild<QAction *>("actionLoad");
    QAction *actionSave = this->findChild<QAction *>("actionSave");
    QAction *actionAbort = this->findChild<QAction *>("actionAbort");
//    QAction *actionCheckSyntax = this->findChild<QAction *>("actionCheck_syntax");
    QAction *actionLoad_Ltl = this->findChild<QAction *>("actionLoad_Ltl");

    connect(actionLoad, SIGNAL(triggered()) , this,SLOT(loadFile()));
    connect(actionSave, SIGNAL(triggered()) , this,SLOT(saveFile()));
    connect(actionAbort, SIGNAL(triggered()),this,SLOT(terminateProcess()));
 //   connect(actionCheckSyntax, SIGNAL(triggered()), this , SLOT(checkSyntax()));
    connect(actionLoad_Ltl, SIGNAL(triggered()), this, SLOT(loadLTLfile()));

    // ## Verify tab ##
    QPushButton *verifyButton = this->findChild<QPushButton *>("buttonVerify");
    // properties
    radioSafety = this->findChild<QRadioButton *>("radioSafety");
    radioAcceptance = this->findChild<QRadioButton *>("radioAcceptance");
    radioLiveness = this->findChild<QRadioButton *>("radioLiveness");
    checkFair = this->findChild<QCheckBox *>("checkFairness");
    connect(verifyButton, SIGNAL(clicked()) , this, SLOT(runVerify()));
    newltlButton = this->findChild<QPushButton *>("newltlButton");

    connect(newltlButton,SIGNAL(clicked()),this,SLOT(newLtl()));
   // connect(ltlList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(runVerify()));


    // ## Simulation tab
    QPushButton *buttonRandomSim = this->findChild<QPushButton *>("buttonRandomSim");
    buttonBackSim = this->findChild<QPushButton*>("buttonSimBackward");
    buttonForwardSim = this->findChild<QPushButton*>("buttonSimForward");
    radioInteractive = this->findChild<QRadioButton*>("radioInteractiveSim");
    radioGuided = this->findChild<QRadioButton*>("radioGuidedSim");
    comboChoice = this->findChild<QComboBox *>("comboChoice");
    spinBoxSteps = this->findChild<QSpinBox *>("spinBoxSteps");
    simulationTypeLabel = this->findChild<QLabel*>("labelSimType");
    fileLabel = this->findChild<QLabel*>("labelSimFile");
    processTable = this->findChild<QTableWidget*>("tableProceses");
    variableTable = this->findChild<QTableWidget*>("tableVariabels");
    connect(buttonRandomSim, SIGNAL(clicked()), this, SLOT(runSimulation()));
    connect(buttonForwardSim, SIGNAL(clicked()),this,SLOT(simulationStepForward()));
    connect(buttonBackSim,SIGNAL(clicked()),this,SLOT(simulationStepBackwards()));
    //connect(variableTabel,SIGNAL(cellChanged(int,int),this,SLOT()));

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
    if (filename!=NULL) fileCleanup();
    QString tempPath = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Promela Files (*.pml)"));
    if (tempPath!=NULL) {
        editor->clear();
        QFile file(tempPath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        status->showMessage("Failed to open "+tempPath);
        } else {
            QTextStream in(&file);
            while(!in.atEnd()) {
                editor->appendPlainText(in.readLine());
            }
            file.close();
            path = tempPath;
            QRegExp rx("/((([a-z]|[A-Z]|\\d)+).pml)");
            rx.indexIn(path);
            filename = rx.cap(1);
            status->showMessage("File loaded: "+path);
        }
    } else {
        status->showMessage("No file chosen");
    }
}

void MainWindow::loadLTLfile(){
    LTLpath = QFileDialog::getOpenFileName(this, tr("Open LTL File"),"",tr("LTL Files (*.ltl)"));
    if (LTLpath!=NULL) {
        QFile LTLfile(LTLpath);

        QRegExp rx("/((([a-z]|[A-Z]|\\d)+).ltl)");
        rx.indexIn(LTLpath);
        filename = rx.cap(1);

        if (!LTLfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        status->showMessage("Failed to open "+LTLpath);
        } else {
            QTextStream in(&LTLfile);
            while(!in.atEnd()) {
                //std::cout << in.readLine().toStdString() << std::endl;
                //ltlList->addItem(in.readLine());
                QString currentLine = in.readLine();
                bool breakout = false;
                for (int i = 0; i < ltlList->count(); i++){
                    if(ltlList->item(i)->text() == currentLine )
                         breakout = true;
                }

                //item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
                //item->setCheckState(Qt::Unchecked); // AND initialize check state
                if(!breakout){
                    QListWidgetItem* item = new QListWidgetItem(currentLine, ltlList);
                    item->setFlags(item->flags() | Qt::ItemIsEditable);
                }
            }
            LTLfile.close();
            status->showMessage("LTL file loaded: "+LTLpath);
        }
    } else {
        status->showMessage("No file chosen");
    }
}

void MainWindow::saveFile() {
    //TODO: Implement functionality to save all LTL's in ltl file
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

//void MainWindow::checkSyntax() {
//    //TODO: Cleanup pan files
//    prepareRun();
//    process = new QProcess();
//    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(checkSyntaxErrorHighlight()));
//    process->start(SPIN,QStringList() << "-a" << path.replace(" ","\\ "));
//}

//void MainWindow::checkSyntaxErrorHighlight() {
//    QString str = process->readAllStandardOutput();
//    QRegExp rxLine("spin: "+path+":(\\d+)");
//    QRegExp rxError("spin: "+path+":\\d+, Error:(\\D*)");
//    QStringList lineNoList;
//    QStringList errorList;
//    int pos = 0;
//    while ((pos = rxError.indexIn(str , (rxLine.indexIn(str, pos)) )) != -1) {
//        QString lineNo = rxLine.cap(1);
//        QString error = rxError.cap(1);
//        lineNoList << lineNo;
//        errorList << error;
//        outputLog->append("Error at line "+lineNo+": "+error);
//        pos += rxLine.matchedLength()+rxError.matchedLength();
//    }
//    editor->HighlightErrorLines(lineNoList);
//}


void MainWindow::runVerify(){
    fileCleanup();
    if (prepareRun()) {
        // COMPILE OPTIONS

        if (radioColapse->isChecked())           compileOpts << "-DCOLLAPSE ";
        else if (radioDH4->isChecked())          compileOpts << "-DH4 ";
        if (radioSafety->isChecked())            compileOpts <<"-DSAFTY ";
        else if (radioLiveness->isChecked())     compileOpts <<"-DNP ";
        compileOpts << "-o" << "pan";
        // TYPE
        verType = VerificationRun::Safety;
        if (radioAcceptance->isChecked())
            verType = VerificationRun::Acceptance;
        if (radioLiveness->isChecked())
            verType = VerificationRun::Liveness;

        // FETCH LTL
        ltl = "";
        if(verType == VerificationRun::Acceptance && ltlList->count() > 0){
           ltl = getLtl();
        }
        spinRun = new syntaxRun(path,ltl,"Verification");
        runProcess(spinRun);

    }
}

void MainWindow::verify(){
    //TODO: Clear verification tab
    if (dynamic_cast<syntaxRun*>(spinRun)-> errors == 0){
        // START VERIFICATION
        spinRun = new VerificationRun(path, verType,checkFair->isChecked(),ltl, compileOpts,spinBoxSDepth->value(),hashSize());
        runProcess(spinRun);
    }else {
        QStringList lineNoList = dynamic_cast<syntaxRun*>(spinRun)->lineNoList;
        QStringList errorList = dynamic_cast<syntaxRun*>(spinRun)->errorList;

        for(int i = 0 ; i < lineNoList.count();i++){
            outputLog->append("Error at line "+lineNoList[i]+": "+errorList[i]);
            if (lineNoList[i].toInt() > editor->blockCount()){
                if (ltlList->item(selectedLtl)){
                    ltlList->item(selectedLtl)->setBackgroundColor(Qt::red);
                }
                lineNoList.removeAt(i);
            }
        }
        if (lineNoList.count() > 0){
            editor->HighlightErrorLines(lineNoList);

        }
    }
}


int MainWindow::hashSize() {
    if (checkHSize->isChecked())
        return spinBoxHSize->value();
    else return -1;
}

QString MainWindow::getLtl(){
    for (int i = 0; i < ltlList->count(); i++){
        if(ltlList->item(i)->isSelected()){
            selectedLtl = i;
            return ltlList->item(i)->text();
        }
    }
    selectedLtl = -1;
    return "";
}



void MainWindow::runSimulation() {
    if (prepareRun()) {
        // TYPE
        simType = SimulationRun::Random;
        if (radioInteractive->isChecked())  {
            simType = SimulationRun::Interactive;
            simulationTypeLabel->setText("Interactive");
        } else if (radioGuided->isChecked()) {
            simType = SimulationRun::Guided;
            simulationTypeLabel->setText("Guided");
        } else simulationTypeLabel->setText("Random");
        fileLabel->setText(filename);
        spinRun = new syntaxRun(path,"","Simulation");
        runProcess(spinRun);
    }
}

void MainWindow::simulation(){
    if (dynamic_cast<syntaxRun*>(spinRun)-> errors == 0){
        // START SIMULATION
        spinRun = new SimulationRun(path,simType,spinBoxSteps->value());
        runProcess(spinRun);
        connect(spinRun,SIGNAL(finished()),this,SLOT(createSimulationTab()));
        buttonForwardSim->setDisabled(false);
    }else {
        QStringList lineNoList = dynamic_cast<syntaxRun*>(spinRun)->lineNoList;
        QStringList errorList = dynamic_cast<syntaxRun*>(spinRun)->errorList;

        for(int i = 0 ; i < lineNoList.count();i++){
            outputLog->append("Error at line "+lineNoList[i]+": "+errorList[i]);
            if (lineNoList[i].toInt() > editor->blockCount()){
                if (ltlList->item(selectedLtl)){
                    ltlList->item(selectedLtl)->setBackgroundColor(Qt::red);
                }
                lineNoList.removeAt(i);
            }
        }
        if (lineNoList.count() > 0){
            editor->HighlightErrorLines(lineNoList);

        }
    }
}

void MainWindow::runProcess(SpinRun* run){
    outputLog->clear();
    thread = new QThread(this);
    run->moveToThread(thread);
    connect(thread,SIGNAL(started()),run,SLOT(start()));
    connect(run, SIGNAL(readReady()),this,SLOT(processReadReady()));
    connect(run, SIGNAL(finished()),this,SLOT(processFinished()));
    connect(run, SIGNAL(finished()), thread, SLOT(quit()));
    connect(run, SIGNAL(statusChanged()),this,SLOT(processStatusChange()));
    connect(run,SIGNAL(syntaxFinishedVer()),this,SLOT(verify()));
    connect(run,SIGNAL(syntaxFinishedSim()),this,SLOT(simulation()));
    connect(thread, SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
}



void MainWindow::simulationStepForward() {
    if (spinRun->type==SpinRun::Simulation) {
        dynamic_cast<SimulationRun*>(spinRun)->goForward();
        UpdateSimulationTab();
    }
}

void MainWindow::simulationStepBackwards() {
    if (spinRun->type==SpinRun::Simulation) {
        dynamic_cast<SimulationRun*>(spinRun)->goBackwards();
        UpdateSimulationTab();
    }
}

void MainWindow::processFinished() {
    outputLog->append(spinRun->readOutput());
    if (spinRun->type==SimulationRun::Verification) {
        processVerificationOutput(outputLog->toPlainText());
    }
    if (checkOptDepth->isChecked()) {
        spinBoxSDepth->setValue((verificationOutput->depth).toInt()+10);
    }
    status->showMessage("Finished");
}

void MainWindow::processReadReady() {
    outputLog->append(spinRun->readOutput());
}

void MainWindow::processStatusChange() {
    status->showMessage(spinRun->readStatus());
}

// returns true if there is a file to run
bool MainWindow::prepareRun(bool clearLog){
    if (path!=NULL) saveFile();
    else loadFile();
    if (clearLog) outputLog->clear();
    return path!=NULL;
}

void MainWindow::terminateProcess(){
    if(thread!= NULL && thread->isRunning()){
        thread->disconnect();
        thread->terminate();
        outputLog->clear();
        status->showMessage("Process killed.");
    }
}

// TODO: Flytte den her så spinRun kan bruge den til at fjerne filer?
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


void MainWindow::newLtl(){
    QListWidgetItem *item = new QListWidgetItem("ltl newName {}" ,ltlList);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void MainWindow::processVerificationOutput(QString output){
    verificationOutput->processVerification(output);
    updateVerificationTab();
}

void MainWindow::UpdateSimulationTab() {
    SimulationRun* simulation = dynamic_cast<SimulationRun*>(spinRun);
    if (simulation->currentStepChangeVariable()) {
        QTableWidgetItem *value = new QTableWidgetItem(simulation->getCurrentVarValue());
        int v_row = simulation->getCurrentVarId();
        variableTable->setItem(v_row,1,value);
    }
    int p_row = simulation->getCurrentProcId();
    processTable->setItem(p_row,1,new QTableWidgetItem(QString::number(simulation->getCurrentProcLine())));
    editor->HighlightProcesses(simulation->getProcs());
    buttonForwardSim->setEnabled(simulation->canGoForward());
    buttonBackSim->setEnabled(simulation->canGoBackwards());
}

void MainWindow::createSimulationTab() {
    SimulationRun* simulation = dynamic_cast<SimulationRun*>(spinRun);
    QList<SimulationRun::variable> variables = simulation->getVariables();
    QList<SimulationRun::proc> procs = simulation->getProcs();
    variableTable->setRowCount(variables.length());
    processTable->setRowCount(procs.length());
    for (int i = 0 ; i < variables.length() ; i++) {
        variableTable->setItem(variables[i].id,0,new QTableWidgetItem(variables[i].name));
        variableTable->setItem(variables[i].id,1,new QTableWidgetItem(variables[i].value));
    }
    for (int i = 0 ; i < procs.length() ; i++) {
        processTable->setItem(procs[i].id,0,new QTableWidgetItem(procs[i].name));
        processTable->setItem(procs[i].id,1,new QTableWidgetItem("-"));
    }
    processTable->resizeColumnsToContents();
    variableTable->resizeColumnsToContents();
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
    hashsizeLabel->setText("Hash size: 2^" + verificationOutput->hashsize);

    // MEMORY USAGE
    statememoryLabel->setText("Memory usage for states: " + verificationOutput->statememory);
    hashmemoryLabel->setText("Memory usage for hash table: " + verificationOutput->hashmemory);
    DFSmemoryLabel->setText("Memory usage for DFS stack: " + verificationOutput->DFSmemory);
    totalmemoryLabel->setText("Total memory usage: " + verificationOutput->totalmemory);

    timestampLabel->setText(verificationOutput->timestamp);


    //IF LTL RUN, update ltl evaluation:

    if(dynamic_cast<VerificationRun*>(spinRun)->verificationType==VerificationRun::Acceptance && ltlList->count() > 0 && selectedLtl != -1){

        if(verificationOutput->errors == "0"){
           ltlList->item(selectedLtl)->setBackgroundColor(Qt::green);
        }
        else{
            ltlList->item(selectedLtl)->setBackgroundColor(Qt::red);
        }
    }
}

