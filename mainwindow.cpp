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
    connect(actionCheckSyntax, SIGNAL(triggered()), this , SLOT(checkSyntax()));
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
   // connect(ltlList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(runVerify())); TODO:Bør vi verificere på dobbeltklik?

    // ## Simulation tab
    QPushButton *buttonRandomSim = this->findChild<QPushButton *>("buttonRandomSim");
    QPushButton *buttonBackSim = this->findChild<QPushButton*>("buttonSimBackward");
    QPushButton *buttonForwardSim = this->findChild<QPushButton*>("buttonSimForward");
    radioInteractive = this->findChild<QRadioButton*>("radioInteractiveSim");
    radioGuided = this->findChild<QRadioButton*>("radioGuidedSim");
    comboChoice = this->findChild<QComboBox *>("comboChoice");
    spinBoxSteps = this->findChild<QSpinBox *>("spinBoxSteps");
    simulationTypeLabel = this->findChild<QLabel*>("labelSimType");
    fileLabel = this->findChild<QLabel*>("labelSimFile");
    connect(buttonRandomSim, SIGNAL(clicked()), this, SLOT(runSimulation()));
    connect(buttonForwardSim, SIGNAL(clicked()),this,SLOT(simulationStepForwards()));
    connect(buttonBackSim,SIGNAL(clicked()),this,SLOT(simulationStepBackwards()));

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

void MainWindow::loadLTLfile(){
    // TODO: Indsæt temporær filepath, når man åbner dialogen overskrives filepath til "" hvis ikke man vælger en ny fil.
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

void MainWindow::checkSyntax() {
    prepareRun();
    process = new QProcess();
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

// BUG: Hvis man spammer verify knappen bliver gamle threads ikke lukker (memory leak)
void MainWindow::runVerify(){
    fileCleanup();
    if (prepareRun()) {
        // COMPILE OPTIONS
        QStringList compileOpts;
        if (radioColapse->isChecked())           compileOpts << "-DCOLLAPSE ";
        else if (radioDH4->isChecked())          compileOpts << "-DH4 ";
        if (radioSafety->isChecked())            compileOpts <<"-DSAFTY ";
        else if (radioLiveness->isChecked())     compileOpts <<"-DNP ";
        compileOpts << "-o" << "pan";
        // TYPE
        VerificationRun::VerificationType type = VerificationRun::Safety;
        if (radioAcceptance->isChecked())
            type = VerificationRun::Acceptance;
        if (radioLiveness->isChecked())
            type = VerificationRun::Liveness;
        // START VERIFICATION
        spinRun = new VerificationRun(path, type,checkFair->isChecked(),compileOpts,spinBoxSDepth->value(),checkHSize->isChecked());
        runProcess(spinRun);
    }
}

void MainWindow::runSimulation() {
    if (prepareRun()) {
        // TYPE
        SimulationRun::SimulationType       type = SimulationRun::Random;
        if (radioInteractive->isChecked())  {
            type = SimulationRun::Interactive;
            simulationTypeLabel->setText("Interactive");
        } else if (radioGuided->isChecked()) {
            type = SimulationRun::Guided;
            simulationTypeLabel->setText("Guided");
        } else simulationTypeLabel->setText("Random");
        fileLabel->setText(filename);
        // START SIMULATION
        spinRun = new SimulationRun(path,type,spinBoxSteps->value());
        runProcess(spinRun);
    }
}

void MainWindow::runProcess(SpinRun* run){
    outputLog->clear();
    QThread* thread = new QThread();
    run->moveToThread(thread);
    connect(thread,SIGNAL(started()),run,SLOT(start()));
    connect(run, SIGNAL(readReady()),this,SLOT(processReadReady()));
    connect(run, SIGNAL(finished()),this,SLOT(processFinished()));
    connect(run, SIGNAL(finished()), thread, SLOT(quit()));
    connect(run, SIGNAL(statusChanged()),this,SLOT(processStatusChange()));
    connect(thread, SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
}


//QStringList MainWindow::getLtlOptions(){
//    QStringList out;
//    if (verificationType() == VerificationRun::Acceptance && ltlList->count()!=0){
//        out << "-f ";
//        for (int i = 0; i < ltlList->count(); i++){
//            //if(ltlList->item(i)->checkState() == Qt::Checked){
//              if(ltlList->item(i)->isSelected()){
//                selectedLtl = i;
//                QRegularExpression re1("(ltl\\s*.*{)");
//                QRegularExpression re2("}");
//                qDebug() << ltlList->item(i)->text();
//                qDebug() << ltlList->item(i)->text().replace(re1,"(").replace(re2,")");
//                out << ltlList->item(i)->text().replace(re1,"!(").replace(re2,")");
//                // JEG STRIPPER NAVNGIVNINGEN, DET ER LIDT ÆRGERLIGT... MON MAN KAN TAGE DEN MED... OM IKKE ANDET, SÅ BARE CONCATENATE MED OUTPUT FOR NEVER CLAIM?
//            }
//        }
//    }

//    return out;

//}

void MainWindow::simulationStepForward() {

    if (spinRun->type==SpinRun::Simulation) {

    }
}

void MainWindow::simulationStepBackwards() {

    if (spinRun->type==SpinRun::Simulation) {

    }
}

void MainWindow::processFinished() {
    outputLog->append(spinRun->readOutput());
    processVerificationOutput(outputLog->toPlainText());
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
    if(process != NULL){
        process->disconnect();
        outputLog->clear();
        status->showMessage("Process killed.");
        process->kill();
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
}//TODO: DEN FATTER IKKE EN SKID HVIS MAN FORSØGER AT VERIFICERER TING DER IKKE GIVER MENING...

void MainWindow::newLtl(){
    QListWidgetItem *item = new QListWidgetItem("Insert new LTL formula here",ltlList);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
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
    hashsizeLabel->setText("Hash size: 2^" + verificationOutput->hashsize);

    // MEMORY USAGE
    statememoryLabel->setText("Memory usage for states: " + verificationOutput->statememory);
    hashmemoryLabel->setText("Memory usage for hash table: " + verificationOutput->hashmemory);
    DFSmemoryLabel->setText("Memory usage for DFS stack: " + verificationOutput->DFSmemory);
    totalmemoryLabel->setText("Total memory usage: " + verificationOutput->totalmemory);

    timestampLabel->setText(verificationOutput->timestamp);

    //TODO: INDSÆT LABEL MED NAVN PÅ FIL
    //IF LTL RUN, update ltl evaluation:
    if(spinRun->type==SpinRun::Verification && ltlList->count() != 0){

        if(verificationOutput->errors == "0"){
           ltlList->item(selectedLtl)->setBackgroundColor(Qt::green);
        }
        else{
            ltlList->item(selectedLtl)->setBackgroundColor(Qt::red);


    }
    }
}
//TODO: EN MÅDE AT IDENTIFICERE HVILKEN TYPE RUN VI LAVER, ER DET ACCEPTANCE ELLER HVAD ER DET, SKAL VI AUTOMATISK KØRE ACCEPTANCE  HVIS MAN DOBBELTKLIKKER EN LTL?
