#include "mainwindow.h"

/* TODO list
 * there is a bug when simulation is at last step and you try to go up
 */

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("QSpin");

    //Setting font of groupbox titles
    // The stylesheet is not inherited to children of the QGroupBox, thereby the labels won't suffer the effect of the change in the parents stylesheet.
    //QGroupBox *statespaceprop = this->findChild<QGroupBox *>("groupBox_4");
    //statespaceprop->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } ");
    //QGroupBox *statespacespecs = this->findChild<QGroupBox *>("groupBox_5");
    //statespacespecs->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } ");
    //QGroupBox *memoryusage = this->findChild<QGroupBox *>("groupBox_6");
    //memoryusage->setStyleSheet("QGroupBox { font-weight: bold; text-decoration: underline; } ");

    // Connecting to objects

    //Statespaceprop groupbox
    spinVerLabel = this->findChild<QLabel *>("spinVerLabel");
    evalLabel = this->findChild<QLabel *>("evalLabel");
    partialLabel = this->findChild<QLabel *>("partialLabel_2");
    neverLabel = this->findChild<QLabel *>("neverLabel_2");
    assertionLabel = this->findChild<QLabel *>("assertionLabel_2");
    acceptanceLabel = this->findChild<QLabel *>("acceptanceLabel_2");
    CycleTypeLabel = this->findChild<QLabel * >("LabelCycleType");
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
    QAction *actionCheckSyntax = this->findChild<QAction *>("actionCheck_syntax");
    QAction *actionLoad_Ltl = this->findChild<QAction *>("actionLoad_Ltl");

    connect(actionLoad, SIGNAL(triggered()) , this,SLOT(loadFile()));
    connect(actionSave, SIGNAL(triggered()) , this,SLOT(saveFile()));
    connect(actionAbort, SIGNAL(triggered()),this,SLOT(terminateProcess()));
    connect(actionCheckSyntax, SIGNAL(triggered()), this , SLOT(runCheckSyntax()));
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
    connect(buttonRandomSim, SIGNAL(clicked()), this, SLOT(runSimulation()));
    connect(buttonForwardSim, SIGNAL(clicked()),this,SLOT(simulationStepForward()));
    connect(buttonBackSim,SIGNAL(clicked()),this,SLOT(simulationStepBackwards()));
    //connect(variableTabel,SIGNAL(cellChanged(int,int),this,SLOT()));

    // Interactive Tab
    QPushButton *buttonInteractiveSim = this->findChild<QPushButton *>("buttonInteractiveSim");
    listChoises = this->findChild<QListWidget *>("choisesList");
    connect(buttonInteractiveSim, SIGNAL(clicked()), this, SLOT(interactiveSimulation()));

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
        // setup SyntaxRun
        syntaxRun = new SyntaxRun(path,ltl);
        QThread* thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(noErrors()),this,SLOT(verify()));
        connect(syntaxRun, SIGNAL(hasErrors()),this,SLOT(displayErrors()));
        thread->start();
    }
}

void MainWindow::verify(){
        clearVerificationTab();
        spinRun = new VerificationRun(path, verType,checkFair->isChecked(),ltl, compileOpts,spinBoxSDepth->value(),hashSize());
        outputLog->clear();
        QThread* thread = connectProcess(spinRun);
        connect(spinRun,SIGNAL(finished()),this,SLOT(updateVerificationTab()));
        thread->start();
}

void MainWindow::interactiveSimulation() {
    if (prepareRun()) {

        processTable = this->findChild<QTableWidget*>("tableProceses_I");
        variableTable = this->findChild<QTableWidget*>("tableVariabels_I");
        simulationSteps = this->findChild<QListWidget*>("listSteps_I");
        fileLabel = this->findChild<QLabel*>("labelSimFile_I");

        simType = SimulationRun::Interactive;
        fileLabel->setText(filename);

        syntaxRun = new SyntaxRun(path,"");
        QThread* thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(noErrors()),this,SLOT(simulation()));
        connect(syntaxRun, SIGNAL(hasErrors()),this,SLOT(displayErrors()));
        thread->start();
    }
}

void MainWindow::runSimulation() {
    if (prepareRun()) {

        processTable = this->findChild<QTableWidget*>("tableProceses");
        variableTable = this->findChild<QTableWidget*>("tableVariabels");
        simulationSteps = this->findChild<QListWidget*>("listSteps");
        fileLabel = this->findChild<QLabel*>("labelSimFile");

        simType = SimulationRun::Random;
        if (radioGuided->isChecked()) {
            simType = SimulationRun::Guided;
            simulationTypeLabel->setText("Guided");
        } else simulationTypeLabel->setText("Random");
        fileLabel->setText(filename);
        syntaxRun = new SyntaxRun(path,"");
        QThread* thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(noErrors()),this,SLOT(simulation()));
        connect(syntaxRun, SIGNAL(hasErrors()),this,SLOT(displayErrors()));
        thread->start();
    }
}

void MainWindow::simulation(){
        spinRun = new SimulationRun(path,simType,spinBoxSteps->value());
        QThread* thread = connectProcess(spinRun);
        connect(spinRun,SIGNAL(readReady()),this,SLOT(createSimulationTab()));
        connect(spinRun,SIGNAL(finished()),this,SLOT(createSimulationTab()));
        connect(spinRun,SIGNAL(readReady()),this,SLOT(processReadReady()));
        connect(listChoises,SIGNAL(activated(QModelIndex)),dynamic_cast<SimulationRun*>(spinRun),SLOT(commitChoise(QModelIndex)));
        connect(simulationSteps,SIGNAL(itemSelectionChanged()),this,SLOT(simulationStepClicked()));
        buttonForwardSim->setDisabled(false);
        thread->start();
}

QThread* MainWindow::connectProcess(SpinRun* run){
    outputLog->clear();
    QThread* thread = new QThread(this);
    run->moveToThread(thread);
    connect(thread,SIGNAL(started()),run,SLOT(start()));
    connect(run, SIGNAL(finished()),this,SLOT(processFinished()));
    connect(run, SIGNAL(finished()), thread, SLOT(quit()));
    connect(run, SIGNAL(statusChanged(SpinRun*)),this,SLOT(processStatusChange(SpinRun*)));
    connect(thread, SIGNAL(finished()),thread,SLOT(deleteLater()));
    return thread;
}

void MainWindow::runCheckSyntax() {
    if(prepareRun()){
        syntaxRun = new SyntaxRun(path,"");
        QThread* thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(hasErrors),this,SLOT(displayErrors()));
        thread->start();
    }
}

void MainWindow::displayErrors() {
    QStringList lineNoList = syntaxRun->lineNoList;
    QStringList errorList = syntaxRun->errorList;

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
    status->showMessage("Finished");
}

void MainWindow::processReadReady() {
    outputLog->append(spinRun->readOutput());
}

void MainWindow::processStatusChange(SpinRun* run) {
    status->showMessage(run->readStatus());
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

void MainWindow::UpdateSimulationTab() {
    SimulationRun* simulation = dynamic_cast<SimulationRun*>(spinRun);
    QList<SimulationRun::variable> variables = simulation->getVariables();
    QList<SimulationRun::proc> procs = simulation->getProcs();
    // Variablse tab
    for (int i = 0 ; i < variables.length() ; i++) {
        variableTable->setItem(variables[i].id,0,new QTableWidgetItem(variables[i].name));
        variableTable->setItem(variables[i].id,1,new QTableWidgetItem(variables[i].value));
    }
    // Process tab
    for (int i = 0 ; i < procs.length() ; i++) {
        int procLine = procs[i].line;
        processTable->setItem(procs[i].id,0,new QTableWidgetItem(procs[i].name));
        if (procLine<0) {
            processTable->setItem(procs[i].id,1,new QTableWidgetItem("-"));
        } else {
            processTable->setItem(procs[i].id,1,new QTableWidgetItem(QString::number(procLine)));
        }
    }
    editor->HighlightProcesses(simulation->getProcs());
    buttonForwardSim->setEnabled(simulation->canGoForward());
    buttonBackSim->setEnabled(simulation->canGoBackwards());
    simulationSteps->item(simulation->getCurrentIndex())->setSelected(true);
}

void MainWindow::createSimulationTab() {
    SimulationRun* simulation = dynamic_cast<SimulationRun*>(spinRun);
    QList<SimulationRun::variable> variables = simulation->getVariables();
    QList<SimulationRun::proc> procs = simulation->getProcs();
    QStringList operations = simulation->getOperations();
    variableTable->setRowCount(variables.length());
    processTable->setRowCount(procs.length());
    simulationSteps->clear();
    // Variablse tab
    for (int i = 0 ; i < variables.length() ; i++) {
        variableTable->setItem(variables[i].id,0,new QTableWidgetItem(variables[i].name));
        variableTable->setItem(variables[i].id,1,new QTableWidgetItem(variables[i].value));
    }
    // Process tab
    for (int i = 0 ; i < procs.length() ; i++) {
        processTable->setItem(procs[i].id,0,new QTableWidgetItem(procs[i].name));
        processTable->setItem(procs[i].id,1,new QTableWidgetItem("-"));
    }
    // Steps list
    foreach (QString operation , operations) {
        simulationSteps->addItem(new QListWidgetItem(operation));
    }
    if (int index = simulation->getCurrentIndex()) {
        simulationSteps->item(index)->setSelected(true);
    }
    // Interactive Choises
    if (simulation->simulationType == SimulationRun::Interactive) {
        listChoises->clear();
        QList<SimulationRun::choise> choises = simulation->getChoises();
        foreach (SimulationRun::choise choise , choises) {
            listChoises->addItem(new QListWidgetItem(choise._proc.name+" | "+choise.operation));
        }
    }
    processTable->resizeColumnsToContents();
    variableTable->resizeColumnsToContents();
}

void MainWindow::simulationStepClicked(){
    SimulationRun* simulation = dynamic_cast<SimulationRun*>(spinRun);
    if (simulation && simulationSteps->selectedItems().length()>0) {
        int newIndex = simulationSteps->selectionModel()->selectedIndexes()[0].row();
        int oldIndex = simulation->getCurrentIndex();
        if (newIndex<oldIndex) {
            simulation->goBackwards(oldIndex-newIndex);
        } else if (newIndex>oldIndex) {
            simulation->goForward(newIndex-oldIndex);
        }
        UpdateSimulationTab();
    }
}

void MainWindow::listChoiseActivated(QModelIndex index) {
    SimulationRun* simulation = dynamic_cast<SimulationRun*>(spinRun);
    if (simulation) {
        QList<SimulationRun::choise> choises = simulation->getChoises();
        SimulationRun::choise choise = choises[index.row()];
    }
}

void MainWindow::updateVerificationTab(){
    VerificationRun* verification = dynamic_cast<VerificationRun*>(spinRun);
    // SPINVERSIONLABEL
    spinVerLabel->setText(verification->spinVer);
    // EVALUATIONLABEL
    evalLabel->setStyleSheet("background-color: " + verification->eval+ ";");
    // STATESPACE PROP
    // PARTIAL ORDER REDUCTION LABEL
    partialLabel->setText(verification->partial);
    // NEVER CLAIM LABEL
    neverLabel->setText(verification->never);
    // ASSERTION LABEL
    assertionLabel->setText(verification->assertion);
    //ACCEPTANCE LABEL
    CycleTypeLabel->setText(verification->acceptanceType.append(" cyles:"));
    acceptanceLabel->setText(verification->acceptance);
    //INVALID END STATES LABEL
    invalidLabel->setText(verification->invalid);
    // STATESPACE SPECS
    errorLabel->setText(verification->errors);
    depthLabel->setText(verification->depth);
    storedstatesLabel->setText(verification->storedStates);
    matchedstatesLabel->setText(verification->matchedStates);
    transitionLabel->setText(verification->transitions);
    atomicLabel->setText(verification->atomic);
    statesizeLabel->setText(verification->statesize);
    hashconflictsLabel->setText(verification->hashconflict);
    hashsizeLabel->setText(verification->hashsize);
    // MEMORY USAGE
    statememoryLabel->setText(verification->statememory);
    hashmemoryLabel->setText(verification->hashmemory);
    DFSmemoryLabel->setText(verification->DFSmemory);
    totalmemoryLabel->setText(verification->totalmemory);

    timestampLabel->setText(verification->timestamp);

    //IF LTL RUN, update ltl evaluation:
    if(dynamic_cast<VerificationRun*>(spinRun)->verificationType==VerificationRun::Acceptance && ltlList->count() > 0 && selectedLtl != -1){
        if(verification->errors == "0"){
           ltlList->item(selectedLtl)->setBackgroundColor(Qt::green);
        }
        else{
            ltlList->item(selectedLtl)->setBackgroundColor(Qt::red);
        }
    }

    // Update search depth value
    if (checkOptDepth->isChecked()) {
        spinBoxSDepth->setValue((verification->depth).toInt()+10);
    }
}

void MainWindow::clearVerificationTab() {
    // Verification tab
    spinVerLabel->clear();
    evalLabel->clear();
    partialLabel->clear();
    neverLabel->clear();
    assertionLabel->clear();
    acceptanceLabel->clear();
    invalidLabel->clear();
    simulationTypeLabel->clear();
    fileLabel->clear();
    errorLabel->clear();
    depthLabel->clear();
    storedstatesLabel->clear();
    matchedstatesLabel->clear();
    transitionLabel->clear();
    atomicLabel->clear();
    statesizeLabel->clear();
    hashconflictsLabel->clear();
    hashsizeLabel->clear();
    statememoryLabel->clear();
    hashmemoryLabel->clear();
    DFSmemoryLabel->clear();
    totalmemoryLabel->clear();
    timestampLabel->clear();
}
