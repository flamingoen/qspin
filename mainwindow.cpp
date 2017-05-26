#include <QFileInfo>
#include <QSettings>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("QSpin");
    loadSettings();

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
    actionCheckSyntax = this->findChild<QAction *>("actionCheck_syntax");
    QAction *actionLoad_Ltl = this->findChild<QAction *>("actionLoad_Ltl");
    QAction *action_about = this->findChild<QAction *>("actionAbout");
    QAction *action_settings = this->findChild<QAction *>("actionSettings");
    QAction *action_saveAs = this->findChild<QAction *>("actionSave_As");
    QAction *action_new = this->findChild<QAction *>("actionNew");
    QAction *action_wordWrap = this->findChild<QAction *>("actionWord_Wrap");
    QAction *action_showHideLog = this->findChild<QAction *>("actionOutput_Log");
    QAction *action_exit = this->findChild<QAction *>("actionExit");
    QAction *action_parserTest = this->findChild<QAction *>("actionParserTest");
    QAction *action_clear_log = this->findChild<QAction *>("actionClear_Log");
    QAction *action_reload = this->findChild<QAction *>("actionReload_File");

    connect(action_new,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(action_about,SIGNAL(triggered()),this,SLOT(showAbout()));
    connect(action_settings,SIGNAL(triggered()),this,SLOT(showSettings()));
    connect(actionLoad, SIGNAL(triggered()) , this,SLOT(loadPmlFile()));
    connect(actionSave, SIGNAL(triggered()) , this,SLOT(saveFile()));
    connect(actionAbort, SIGNAL(triggered()),this,SLOT(terminateProcess()));
    connect(actionCheckSyntax, SIGNAL(triggered()), this , SLOT(runCheckSyntax()));
    connect(actionLoad_Ltl, SIGNAL(triggered()), this, SLOT(loadLtlFile()));
    connect(action_saveAs, SIGNAL(triggered()),this,SLOT(saveFileAs()));
    connect(action_exit, SIGNAL(triggered()),this,SLOT(exit()));
    connect(action_parserTest, SIGNAL(triggered()),this,SLOT(parserTest()));
    connect(action_clear_log, SIGNAL(triggered()),this,SLOT(clearLog()));
    connect(action_reload, SIGNAL(triggered()),this,SLOT(reloadFile()));


    // ## Verify tab ##
    buttonVerify = this->findChild<QPushButton *>("buttonVerify");
    // properties
    radioSafety = this->findChild<QRadioButton *>("radioSafety");
    radioAcceptance = this->findChild<QRadioButton *>("radioAcceptance");
    radioLiveness = this->findChild<QRadioButton *>("radioLiveness");
    checkFair = this->findChild<QCheckBox *>("checkFairness");
    connect(buttonVerify, SIGNAL(clicked()) , this, SLOT(runVerify()));
    newltlButton = this->findChild<QPushButton *>("newltlButton");
    deleteltlButton = this->findChild<QPushButton*>("deleteltlButton");

    connect(newltlButton,SIGNAL(clicked()),this,SLOT(newLtl()));
    connect(deleteltlButton,SIGNAL(clicked()),this,SLOT(deleteLtl()));


    // ## Simulation tab
    buttonRandomSim = this->findChild<QPushButton *>("buttonRandomSim");
    buttonBackSim = this->findChild<QPushButton*>("buttonSimBackward");
    buttonForwardSim = this->findChild<QPushButton*>("buttonSimForward");
    radioInteractive = this->findChild<QRadioButton*>("radioInteractiveSim");
    radioGuided = this->findChild<QRadioButton*>("radioGuidedSim");
    spinBoxSteps = this->findChild<QSpinBox *>("spinBoxSteps");
    simulationTypeLabel = this->findChild<QLabel*>("labelSimType");
    fileLabel = this->findChild<QLabel*>("labelSimFile");
    connect(buttonRandomSim, SIGNAL(clicked()), this, SLOT(runSimulation()));
    connect(buttonForwardSim, SIGNAL(clicked()),this,SLOT(simulationStepForward()));
    connect(buttonBackSim,SIGNAL(clicked()),this,SLOT(simulationStepBackwards()));
    processTable = this->findChild<QTableWidget*>("tableProceses");
    variableTable = this->findChild<QTableWidget*>("tableVariabels");
    simulationSteps = this->findChild<QListWidget*>("listSteps");

    // Interactive Tab
    buttonInteractiveSim = this->findChild<QPushButton *>("buttonInteractiveSim");
    listChoises = this->findChild<QListWidget *>("choisesList");
    connect(buttonInteractiveSim, SIGNAL(clicked()), this, SLOT(runInteractive()));
    processTable_I = this->findChild<QTableWidget*>("tableProceses_I");
    variableTable_I = this->findChild<QTableWidget*>("tableVariabels_I");
    simulationSteps_I = this->findChild<QListWidget*>("listSteps_I");
    fileLabel_I = this->findChild<QLabel*>("labelSimFile_I");

    // options
    checkHSize = this->findChild<QCheckBox *>("checkHashSize");
    spinBoxHSize = this->findChild<QSpinBox *>("spinBoxHashSize");
    spinBoxSDepth = this->findChild<QSpinBox *>("spinBoxSearchDepth");
    checkOptDepth = this->findChild<QCheckBox *>("checkOptimizeDepth");
    checkSaveDepth = this->findChild<QCheckBox *>("checkAutoDepth");
    comboBoxSearch = this->findChild<QComboBox *>("comboBoxSearchType");
    comboBoxMem = this->findChild<QComboBox *>("comboBoxMemComp");

    // ## other ##
    status = this->findChild<QStatusBar *>("statusbar");
    outputLog = this->findChild<QTextEdit *>("log");
    editor = (CodeEditor*) this->findChild<QPlainTextEdit *>("editor");
    connect(editor,SIGNAL(textChanged()),this,SLOT(editorTextChanged()));
    groupBox_log = this->findChild<QGroupBox *>("logBox");
    connect(action_wordWrap, SIGNAL(triggered(bool)),editor,SLOT(setWordWrap(bool)));
    connect(action_showHideLog, SIGNAL(triggered(bool)),this,SLOT(showHideLog(bool)));

    new Highlighter(editor->document());
    iconFallback();
}

MainWindow::~MainWindow() {
    saveSettings();
    fileCleanup();
    delete ui;
}

void MainWindow::loadSettings() {
    // Load the previous window state and geometry.
    QSettings opts;
    restoreGeometry(opts.value("MainWindow/geometry").toByteArray());
    restoreState(opts.value("MainWindow/state").toByteArray());
}

void MainWindow::saveSettings() {
    // Save the window state and geometry.
    QSettings opts;
    opts.setValue("MainWindow/geometry", saveGeometry());
    opts.setValue("MainWindow/state", saveState());
    settings->saveSettings();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (saveWarning()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile() {
    if (saveWarning()){
        fileCleanup();
        path = nullptr;
        clearUI();
        editor->clear();
        editor->document()->setModified(false);
    }
}

void MainWindow::reloadFile() {
    if (saveWarning()) {
        if (tempFilePath!=NULL) {
            editor->clear();
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                status->showMessage("Failed to open "+path);
            } else {
                QTextStream in(&file);
                while(!in.atEnd()) {
                    editor->appendPlainText(in.readLine());
                }
                file.close();
                editor->document()->setModified(false);
                status->showMessage("File reloaded: "+path);
            }
        }else status->showMessage("Warning: No file to reload");
    }
}

void MainWindow::loadPmlFile() {
    if (saveWarning()){
        if (filename!=NULL) {
            fileCleanup();
            clearVerificationTab();
            clearSimulationTab();
            clearInteractiveTab();
        }
        QString tempPath = showOpenFileDialog(
            tr("Open File"),
            tr("Promela Files (*.pml);;All files (*)")
        );
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
                clearUI();
                editor->document()->clearUndoRedoStacks();
                editor->document()->setModified(false);
                status->showMessage("File loaded: "+path);
            }
        } else {
            status->showMessage("No file chosen");
        }
    }
}

void MainWindow::loadLtlFile(){
    LTLpath = showOpenFileDialog(tr("Open LTL File"),tr("LTL Files (*.ltl);;All Files (*)"));
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

void MainWindow::saveLtlFile(){
    if (LTLpath == NULL){
        if (path != NULL){
            int lastPoint = path.lastIndexOf(".");
            QString pathNameNoExt = path.left(lastPoint);
            LTLpath = pathNameNoExt + ".ltl";
        }else { path = QFileDialog::getSaveFileName(this, tr("Save File"),"",tr("LTL Files (*.ltl);;All Files (*)")); }

    }
    QFile file(LTLpath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        status->showMessage("Could not save "+LTLpath);
    } else {
        QTextStream out(&file);
        for (int i = 0; i < ltlList->count(); i++){
            out << ltlList->item(i)->text()+ "\n" ;
        }

        file.close();
        status->showMessage("File saved: "+LTLpath);
    }

}

void MainWindow::saveFileAs() {
    QString tempPath = QFileDialog::getSaveFileName(this, tr("Save File"),"",tr("Promela Files (*.pml);;All Files (*)"));
    if (tempPath!=NULL) {
        path = tempPath;
        saveFile();
    }
}

void MainWindow::saveFile() {
    //TODO: Implement functionality to save all LTL's in ltl file
    if (path==NULL) saveFileAs();
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        status->showMessage("Could not save "+path);
    } else {
        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();
        editor->document()->setModified(false);
        status->showMessage("File saved: "+path);
    }
}

bool MainWindow::saveWarning() {
    if (editor->document()->isModified()) {
        int ans;
        ans = QMessageBox::warning(this, tr("document has been modified"),tr("The document has been modified.\nDo you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
        if (ans==QMessageBox::Save){
            saveFile();
        }
        return !(ans==QMessageBox::Cancel);
    } else return true;
}

QString MainWindow::showOpenFileDialog(QString caption, QString filter)
{
    QSettings settings;
    QString directory = settings.value("FileDialogPreviousDirectory").toString();

    QString fileName = QFileDialog::getOpenFileName(
        this,
        caption,
        directory,
        filter
    );

    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        settings.setValue("FileDialogPreviousDirectory", info.absoluteDir().path());
    }

    return fileName;
}

void MainWindow::runVerify(){
    fileCleanup();
    if (prepareRun()) {
        compileOpts.clear();
        // COMPILE OPTIONS
        if      (comboBoxMem->currentIndex()==1 )   compileOpts << "-DCOLLAPSE ";
        else if (comboBoxMem->currentIndex()==2 )   compileOpts << "-DH4 ";
        if      (radioSafety->isChecked())          compileOpts <<"-DSAFETY ";
        else if (radioLiveness->isChecked())        compileOpts <<"-DNP ";
        if      (comboBoxSearch->currentIndex()==1) compileOpts << "-DBFS";
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
        syntaxRun = new SyntaxRun(settings->getSpin(), tempFilePath, tempFilename, ltl);
        thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(noErrors()),this,SLOT(verify()));
        connect(syntaxRun, SIGNAL(finished(SpinRun*)),this,SLOT(displayErrors()));
        thread->start();
    }
}

void MainWindow::verify(){
        clearVerificationTab();
        verificationRun = new VerificationRun(settings->getSpin(), settings->getCompiler(), tempFilePath, tempFilename, verType,checkFair->isChecked(),ltl, compileOpts,spinBoxSDepth->value(),hashSize(),checkSaveDepth->isChecked());
        outputLog->clear();
        thread = connectProcess(verificationRun);
        connect(this,SIGNAL(closeProcess()),verificationRun,SLOT(terminateProcess())); // Signal for terminating the process running Spin
        connect(verificationRun,SIGNAL(finished(SpinRun*)),this,SLOT(updateVerificationTab()));
        thread->start();
}

void MainWindow::runInteractive() {
    if (prepareRun()) {

        fileLabel_I->setText(QDir::toNativeSeparators(filename));

        syntaxRun = new SyntaxRun(settings->getSpin(), tempFilePath,tempFilename, "");
        thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(noErrors()),this,SLOT(interactive()));
        connect(syntaxRun, SIGNAL(finished(SpinRun*)),this,SLOT(displayErrors()));
        thread->start();
    }
}

void MainWindow::runSimulation() {
    if (prepareRun()) {

        simType = SimulationRun::Random;
        if (radioGuided->isChecked()) {
            simType = SimulationRun::Guided;
            simulationTypeLabel->setText("Guided");
        } else simulationTypeLabel->setText("Random");
        fileLabel->setText(QDir::toNativeSeparators(filename));
        syntaxRun = new SyntaxRun(settings->getSpin(), tempFilePath,tempFilename,"");
        thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(noErrors()),this,SLOT(simulation()));
        connect(syntaxRun, SIGNAL(finished(SpinRun*)),this,SLOT(displayErrors()));
        thread->start();
    }
}

void MainWindow::simulation(){
    simulationRun = new SimulationRun(settings->getSpin(), tempFilePath,tempFilename,simType,spinBoxSteps->value());
    thread = connectProcess(simulationRun);
    connect(this,SIGNAL(closeProcess()),simulationRun,SLOT(terminateProcess())); // Signal for terminating the process running Spin
    //connect(simulationRun,SIGNAL(readReady(SpinRun*)),this,SLOT(createSimulationTab()));
    connect(simulationRun,SIGNAL(finished(SpinRun*)),this,SLOT(createSimulationTab()));
    //connect(simulationRun,SIGNAL(readReady(SpinRun*)),this,SLOT(processReadReady(SpinRun*)));
    connect(simulationSteps,SIGNAL(itemSelectionChanged()),this,SLOT(simulationStepClicked()));
    buttonForwardSim->setDisabled(false);
    thread->start();
}

void MainWindow::interactive(){
    interactiveRun = new SimulationRun(settings->getSpin(), tempFilePath,tempFilename,SimulationRun::Interactive,spinBoxSteps->value());
    thread = connectProcess(interactiveRun);
    connect(this,SIGNAL(closeProcess()),interactiveRun,SLOT(terminateProcess())); // Signal for terminating the process running Spin
    connect(interactiveRun,SIGNAL(readReady(SpinRun*)),this,SLOT(createInteractiveTab()));
    //connect(interactiveRun,SIGNAL(finished(SpinRun*)),this,SLOT(createInteractiveTab()));
    connect(interactiveRun,SIGNAL(readReady(SpinRun*)),this,SLOT(processReadReady(SpinRun*)));
    connect(listChoises,SIGNAL(activated(QModelIndex)),interactiveRun,SLOT(commitChoise(QModelIndex)));
    connect(simulationSteps_I,SIGNAL(itemSelectionChanged()),this,SLOT(interactiveStepClicked()));
    buttonForwardSim->setDisabled(false);
    thread->start();
}

QThread* MainWindow::connectProcess(SpinRun* run){
    outputLog->clear();
    thread = new QThread(this);
    run->moveToThread(thread);
    connect(thread,SIGNAL(started()),run,SLOT(start()));
    connect(run, SIGNAL(finished(SpinRun*)),this,SLOT(processFinished(SpinRun*)));
    connect(run, SIGNAL(finished(SpinRun*)), thread, SLOT(quit()));
    connect(run, SIGNAL(statusChanged(SpinRun*)),this,SLOT(processStatusChange(SpinRun*)));
    connect(run, SIGNAL(processError(QString)),this,SLOT(processError(QString)));
    //connect(thread, SIGNAL(finished()),thread,SLOT(deleteLater()));
    //connect(thread,SIGNAL(started()),this,SLOT(disableRunButtons()));
    disableRunButtons(); // hotfix to fix wrong order of signals - making sure this runs first!
    //connect(thread,SIGNAL(),run, SLOT(deleteLater())); // INDSAT FOR AT FÅ PROCESSEN TIL AT LUKKE
    connect(run,SIGNAL(finished(SpinRun*)),this,SLOT(enableRunButtons()));

    return thread;
}

void MainWindow::processError(QString error) {
    status->showMessage(error);
    outputLog->append(error);
    terminateProcess();

}

void MainWindow::runCheckSyntax() {
    if(prepareRun()){
        syntaxRun = new SyntaxRun(settings->getSpin(), tempFilePath,tempFilename,"");
        thread = connectProcess(syntaxRun);
        connect(syntaxRun, SIGNAL(finished(SpinRun*)),this,SLOT(displayErrors()));
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
    editor->HighlightErrorLines(lineNoList);
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

void MainWindow::disableRunButtons(){
    buttonVerify->setDisabled(true);
    buttonRandomSim->setDisabled(true);
    buttonInteractiveSim->setDisabled(true);
    actionCheckSyntax->setDisabled(true);
}

void MainWindow::enableRunButtons(){
    buttonVerify->setEnabled(true);
    buttonRandomSim->setEnabled(true);
    buttonInteractiveSim->setEnabled(true);
    actionCheckSyntax->setEnabled(true);
}

void MainWindow::simulationStepForward() {
        simulationRun->goForward();
        updateSimulationTab(simulationRun,variableTable,processTable,simulationSteps);
}

void MainWindow::simulationStepBackwards() {
        simulationRun->goBackwards();
        updateSimulationTab(interactiveRun,variableTable_I,processTable_I,simulationSteps_I);
}

void MainWindow::processFinished(SpinRun* run) {
    outputLog->append(run->readOutput());
    status->showMessage("Finished");
}

void MainWindow::processReadReady(SpinRun* run) {
    outputLog->append(run->readOutput());
}

void MainWindow::processStatusChange(SpinRun* run) {
    QString statusmsg = run->readStatus();
    status->showMessage(statusmsg);
    outputLog->append(statusmsg);
}

// returns true if there is a file to run
bool MainWindow::prepareRun(bool clearLog){
    //if (path!=NULL) saveFile();
    //else loadPmlFile();
    if (ltlList->count() > 0) saveLtlFile();
    if (clearLog) outputLog->clear();
    if (tempDir.isValid()){
        QFile file(tempFilePath+QDir::separator()+tempFilename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);
            out << editor->toPlainText();
            file.close();
        }
    }
    return 1;
}

void MainWindow::terminateProcess(){
    emit closeProcess();
    listChoises->clear();
    enableRunButtons();
}

// TODO: Flytte den her så spinRun kan bruge den til at fjerne filer?
void MainWindow::fileCleanup(){
    QDir dir(tempDir.path());
    dir.setNameFilters(QStringList() << "pan*" << "*.trail");
    dir.setFilter(QDir::Files);
    dir.remove(filename);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}

void MainWindow::newLtl(){
    QListWidgetItem *item = new QListWidgetItem("ltl newName {}" ,ltlList);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void MainWindow::deleteLtl(){
    for (int i = 0; i < ltlList->count(); i++){
        if(ltlList->item(i)->isSelected()){
            ltlList->takeItem(i);
        }
    }
}

void MainWindow::updateSimulationTab(SimulationRun *run, QTableWidget *variableTable, QTableWidget *processTable, QListWidget *stepList) {
    QList<Variable*> variables = run->getVariables();
    QList<SimulationRun::proc> procs = run->getProcs();
    // Variablse tab
    for (int i = 0 ; i < variables.length() ; i++) {
        variableTable->setItem(variables[i]->getId(),0,new QTableWidgetItem(variables[i]->getName()));
        variableTable->setItem(variables[i]->getId(),1,new QTableWidgetItem(variables[i]->getValueString()));
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
    editor->HighlightProcesses(run->getProcs());
    stepList->item(run->getCurrentIndex())->setSelected(true);
}

void MainWindow::createSimulationTab() {
    populateSimulationLists(simulationRun,variableTable,processTable,simulationSteps);
}

void MainWindow::createInteractiveTab() {
    populateSimulationLists(interactiveRun,variableTable_I,processTable_I,simulationSteps_I);
    // Interactive Choises
    listChoises->clear();
    QList<SimulationRun::choise> choises = interactiveRun->getChoises();
    foreach (SimulationRun::choise choise , choises) {
        if (choise._proc.name=="")  listChoises->addItem(new QListWidgetItem(choise.operation));
        else                        listChoises->addItem(new QListWidgetItem(choise._proc.name+" | "+choise.operation));
    }
    processTable->resizeColumnsToContents();
    variableTable->resizeColumnsToContents();
}

void MainWindow::simulationStepClicked(){
    updateStepList(simulationSteps, simulationRun);
    updateSimulationTab(simulationRun,variableTable,processTable,simulationSteps);
    buttonForwardSim->setEnabled(simulationRun->canGoForward());
    buttonBackSim->setEnabled(simulationRun->canGoBackwards());
}

void MainWindow::interactiveStepClicked(){
    updateStepList(simulationSteps_I, interactiveRun);
    updateSimulationTab(interactiveRun,variableTable_I,processTable_I,simulationSteps_I);
}

void MainWindow::populateSimulationLists(SimulationRun* run, QTableWidget* variableTable, QTableWidget* processTable, QListWidget *stepList){
    QList<Variable*> variables = run->getVariables();
    QList<SimulationRun::proc> procs = run->getProcs();
    QStringList operations = run->getOperations();
    variableTable->setRowCount(variables.length());
    processTable->setRowCount(procs.length());
    stepList->clear();
    // Variablse tab
    for (int i = 0 ; i < variables.length() ; i++) {
        variableTable->setItem(variables[i]->getId(),0,new QTableWidgetItem(variables[i]->getName()));
        variableTable->setItem(variables[i]->getId(),1,new QTableWidgetItem(variables[i]->getValueString()));
    }
    // Process tab
    for (int i = 0 ; i < procs.length() ; i++) {
        processTable->setItem(procs[i].id,0,new QTableWidgetItem(procs[i].name));
        processTable->setItem(procs[i].id,1,new QTableWidgetItem("-"));
    }
    // Steps list
    foreach (QString operation , operations) {
        stepList->addItem(new QListWidgetItem(operation));
    }
    if (int index = run->getCurrentIndex()) {
        stepList->item(index)->setSelected(true);
    }
    processTable->resizeColumnsToContents();
    variableTable->resizeColumnsToContents();
}

void MainWindow::updateStepList(QListWidget *stepList , SimulationRun *run) {
    if (run && stepList->selectedItems().length()>0) {
        int newIndex = stepList->selectionModel()->selectedIndexes()[0].row();
        int oldIndex = run->getCurrentIndex();
        if (newIndex<oldIndex) {
            run->goBackwards(oldIndex-newIndex);
        } else if (newIndex>oldIndex) {
            run->goForward(newIndex-oldIndex);
        }
    }
}

void MainWindow::listChoiseActivated(QModelIndex index) {
    if (simulationRun) {
        QList<SimulationRun::choise> choises = simulationRun->getChoises();
        SimulationRun::choise choise = choises[index.row()];
    }
}

void MainWindow::updateVerificationTab(){
    // SPINVERSIONLABEL
    spinVerLabel->setText(verificationRun->spinVer);
    // EVALUATIONLABEL
    evalLabel->setStyleSheet("background-color: " + verificationRun->eval+ ";");
    // STATESPACE PROP
    // PARTIAL ORDER REDUCTION LABEL
    partialLabel->setText(verificationRun->partial);
    // NEVER CLAIM LABEL
    neverLabel->setText(verificationRun->never);
    // ASSERTION LABEL
    assertionLabel->setText(verificationRun->assertion);
    //ACCEPTANCE LABEL
    CycleTypeLabel->setText(verificationRun->getAcceptanceType().append(" cyles:"));
    acceptanceLabel->setText(verificationRun->acceptance);
    //INVALID END STATES LABEL
    invalidLabel->setText(verificationRun->invalid);
    // STATESPACE SPECS
    errorLabel->setText(verificationRun->errors);
    depthLabel->setText(verificationRun->depth);
    storedstatesLabel->setText(verificationRun->storedStates);
    matchedstatesLabel->setText(verificationRun->matchedStates);
    transitionLabel->setText(verificationRun->transitions);
    atomicLabel->setText(verificationRun->atomic);
    statesizeLabel->setText(verificationRun->statesize);
    hashconflictsLabel->setText(verificationRun->hashconflict);
    hashsizeLabel->setText(verificationRun->hashsize);
    // MEMORY USAGE
    statememoryLabel->setText(verificationRun->statememory);
    hashmemoryLabel->setText(verificationRun->hashmemory);
    DFSmemoryLabel->setText(verificationRun->DFSmemory);
    totalmemoryLabel->setText(verificationRun->totalmemory);

    timestampLabel->setText(verificationRun->timestamp);

    //IF LTL RUN, update ltl evaluation:
    if(verificationRun->verificationType==VerificationRun::Acceptance && ltlList->count() > 0 && selectedLtl != -1){
        if(verificationRun->errors == "0"){
           ltlList->item(selectedLtl)->setBackgroundColor(Qt::green);
        }
        else{
            ltlList->item(selectedLtl)->setBackgroundColor(Qt::red);
        }
    }

    // Update search depth value
    if (checkOptDepth->isChecked()) {
        spinBoxSDepth->setValue((verificationRun->depth).toInt()+10);
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
    spinBoxSDepth->setValue(10000);
}

void MainWindow::clearSimulationTab() {
    simulationSteps->clear();
    processTable->clearContents();
    variableTable->clearContents();
}

void MainWindow::clearInteractiveTab() {
    simulationSteps_I->clear();
    processTable_I->clearContents();
    variableTable_I->clearContents();
    listChoises->clear();
}

void MainWindow::clearLog() { outputLog->clear(); }

void MainWindow::clearUI() {
    clearInteractiveTab();
    clearSimulationTab();
    clearVerificationTab();
    clearLog();
}

void MainWindow::iconFallback() {
    if (QString::compare("linux",OS)) { // will set icons if system is NOT linux
        QIcon loadFile;
        QIcon saveFile;
        QIcon abort;
        QIcon forward;
        QIcon backward;

        loadFile.addFile(":icons/document-open",QSize(24, 24));
        saveFile.addFile(":icons/document-save",QSize(24, 24));
        abort.addFile(":icons/process-stop",QSize(24, 24));
        forward.addFile(":icons/go-next",QSize(24, 24));
        backward.addFile(":icons/go-previous",QSize(24, 24));

        ui->actionLoad->setIcon(loadFile);
        ui->actionSave->setIcon(saveFile);
        ui->actionAbort->setIcon(abort);
        ui->buttonSimForward->setIcon(forward);
        ui->buttonSimBackward->setIcon(backward);
    }
}

void MainWindow::showAbout() {
    QDialog* about = new QDialog(0,0);
    Ui_About aboutUi;
    aboutUi.setupUi(about);
    about->show();
}

void MainWindow::showSettings() {
    settings->show();
}

void MainWindow::showHideLog(bool show) {
    if (show)   groupBox_log->show();
    else        groupBox_log->hide();
}

void MainWindow::exit() {
    QCoreApplication::exit();
}

bool MainWindow::parserTest() {
    std::string value = editor->toPlainText().remove(QRegularExpression("\\/\\*(?:.|\\n)*?\\*\\/|\\/\\/.*?\\\n")).toStdString();
    std::string::const_iterator iter = value.begin();
    std::string::const_iterator end = value.end();
    std::vector<std::vector<std::string>> parseList;
    bool res;
    res  = phrase_parse(iter,end,grammar,space,parseList);
    if (res) {
        for (uint i=0 ; i<parseList.size() ; i++) {
            cout << "[ ";
            for(uint j=0 ; j<parseList[i].size() ; j++) {
                   cout << parseList[i][j] << " ";
            } cout << " ] ";
        } cout << "\n";
    }
    if ( res ) {
        outputLog->append("Parsing succesfull");
    } else {
        outputLog->append("Parsing failed");
    }
    return res;
}

void MainWindow::editorTextChanged() {
    parserTest();
}
