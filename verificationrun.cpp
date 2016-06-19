#include "verificationrun.h"

/* TODO list for verification
 * merge med verification output (hvorfor have to?)
 * tilføje en error list, der fortæller hvilke fejl man har
 * Husk exception handling til at (især vores regex)
 * SMID I TRY CATCH - hvis der sker en fejl, indsæt fejl i SPINVER label hvor der står at man bliver nødt til at bruge rawlog
 * Unable fairness knappen når den alligevel ikke kan bruges (Safety??)
 * Tilføj en remove ltl knap
 */

VerificationRun::VerificationRun(QString _path, VerificationType _type, bool _fairness, QString _ltl, QStringList _compileOptions, int _searchDepth, int _hashSize) : SpinRun(_path , Verification){
    verificationType = _type;
    fairness = _fairness;
    ltl = _ltl;
    compileOptions = _compileOptions;
    searchDepth = _searchDepth;
    hashSize = _hashSize;
}

VerificationRun::~VerificationRun() {
    process->deleteLater();
}

void VerificationRun::start(){
    process = new QProcess();
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(runCompile()));
    setStatus("Verification: Creating model");

    // INSERT LTL IF ACCEPTANCE RUN
    if(verificationType == Acceptance){
     tempPath = createTempPml();
     process->start(SPIN,QStringList() << "-a" << "\""+tempPath+"\"");
    }
    else process->start(SPIN,QStringList() << "-a" << "\""+path+"\"");
}


QString VerificationRun::createTempPml(){
    QFile::copy(path,path+".qspin");
    QFile tempFile(path+".qspin");

    if ( tempFile.open(QIODevice::Append | QIODevice::ReadWrite) )
    {
        QTextStream stream( &tempFile );
        stream << "\n " + ltl << endl;
    }
    return path+".qspin";
}


void VerificationRun::runCompile(){
    QFile::remove(tempPath); // Removing the temporary file
    process = new QProcess();
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(runPan()));
    setStatus("Verification: Compiling pan.c");
    QStringList options = compileOptions;
    options << "-DNFAIR="+QString::number(nFair);
    process->start(CCOMPILER, options << "pan.c");
}

void VerificationRun::runPan(){
    process = new QProcess();
    connect(process, SIGNAL(readyReadStandardOutput()),this,SLOT(readReadyVerification()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedVerification()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), process, SLOT(deleteLater()));

    QStringList runOptions;
    switch (verificationType) {
    case Safety:
        break;
    case Acceptance:    runOptions << "-a";
        break;
    case Liveness:      runOptions << "-l";
        break;
    }
    if (searchDepth>0)  runOptions << "-m"+QString::number(searchDepth);
    if (hashSize>0)     runOptions << "-w"+QString::number(hashSize);
    if (fairness)       runOptions << "-f";
    setStatus("Verification: Running verification");
    process->start("./pan", runOptions);
}

void VerificationRun::finishedVerification() {
    currentOutput.append(process->readAllStandardOutput());
    processVerification(currentOutput);
    emit finished(this);
}

void VerificationRun::readReadyVerification() {
    QRegExp sd("error: max search depth too small");
    QRegExp nf("error: too many processes");
    QString newOutput = process->readAllStandardOutput();
    if (newOutput.contains(nf)) {
        nFair += 1;
        process->disconnect();
        runCompile();
        setStatus("restarted with DNFAIR="+QString::number(nFair));
    } else if (newOutput.contains(sd)) {
        if (searchDepth != -1) {
            searchDepth += (searchDepth/2);
            setStatus("restarted with search depth ="+QString::number(searchDepth));
        }
        else searchDepth = 15000;
        currentOutput.append("max search depth too small: restarting with new depth = "+QString::number(searchDepth) + "\n");
        process->disconnect();
        runPan();
    } else {
        currentOutput.append(newOutput);
    }
    emit readReady(this);
}

void VerificationRun::processVerification(QString output){
    // TIMESTAMP
    QDateTime * time = new QDateTime();
    timestamp = (time->currentDateTimeUtc().toLocalTime()).toString();

    // SPIN VERSION
    QRegularExpression re("(Spin Version .+)\\)");
    QRegularExpressionMatch spinVerM = re.match(output);
    spinVer = spinVerM.captured(1);

    // EVALUATION
    re.setPattern("Warning");
    QRegularExpressionMatch warningM = re.match(output);

    re.setPattern("errors: (\\d)");
    QRegularExpressionMatch evalM = re.match(output);

    if(evalM.captured(1) == "0" && !warningM.hasMatch())        eval = "GREEN";
    else if (evalM.captured(1) == "0" && warningM.hasMatch())   eval = "YELLOW";
    else                                                        eval = "RED";

    // STATESPACEPROP *******************************************************************************************************'
    re.setPattern("([-+])\\s*Partial Order Reduction");
    QRegularExpressionMatch partialM = re.match(output);
    if (partialM.hasMatch()) partial = partialM.captured(1);


    re.setPattern("never\\s*claim\\s*(.+)");
    QRegularExpressionMatch neverM = re.match(output);
    never = neverM.captured(1);

    re.setPattern("assertion\\s*violations\\s*(.+)");
    QRegularExpressionMatch assertionM = re.match(output);
    assertion = assertionM.captured(1);

    re.setPattern("(\\S*)\\s*cycles\\s*(.+)");
    QRegularExpressionMatch acceptanceM = re.match(output);
    acceptanceType = acceptanceM.captured(1).replace(0,1,acceptanceM.captured(1)[0].toUpper()); // TO ENSURE THAT THE FIRST CHARACTER IS UPPERCASE.
    acceptance = acceptanceM.captured(2);

    re.setPattern("invalid\\s+end\\s*states\\s*(.+)");
    QRegularExpressionMatch invalidM = re.match(output);
    invalid = invalidM.captured(1);

    // STATESPACESPECS ***************************************************************************************************
    // ERRORS - already used in the evaluation above.
    errors = evalM.captured(1);

    // DEPTH
    re.setPattern("depth reached ([0-9+e.]*)");
    QRegularExpressionMatch depthM = re.match(output);
    depth = depthM.captured(1);

    // STORED STATES
    re.setPattern("([0-9+e.]*) states, stored");
    QRegularExpressionMatch storedStatesM = re.match(output);
    storedStates = storedStatesM.captured(1);

    // MATCHED STATES
    re.setPattern("([0-9+e.]*) states, matched");
    QRegularExpressionMatch matchedStatesM = re.match(output);
    matchedStates = matchedStatesM.captured(1);

    // TRANSITIONS
    re.setPattern("([0-9+e.]*) transitions");
    QRegularExpressionMatch transitionsM = re.match(output);
    transitions = transitionsM.captured(1);

    // ATOMIC STATES
    re.setPattern("([0-9+e.]*) atomic steps");
    QRegularExpressionMatch atomicM = re.match(output);
    atomic = atomicM.captured(1);

    // STATE SIZE
    re.setPattern("State-vector ([0-9+e.]*) byte");
    QRegularExpressionMatch statesizeM = re.match(output);
    statesize = statesizeM.captured(1);

    // HASH CONFLICTS
    re.setPattern("hash conflicts:\\s* ([0-9+e.]*)");
    QRegularExpressionMatch hashconflictM = re.match(output);
    hashconflict = hashconflictM.captured(1);

    // HASH SIZE
    re.setPattern("memory used for hash table \\(-w(\\d+)\\)");
    QRegularExpressionMatch hashsizeM = re.match(output);
    hashsize = hashsizeM.captured(1);

    // Memory usage **********************************************************************************

    //STATE MEMORY
    re.setPattern("(\\d+\\.\\d+)\\s+actual\\s+memory\\s+usage\\s+for\\s+states");
    QRegularExpressionMatch statememoryM = re.match(output);
    statememory = statememoryM.captured(1);

    //HASH MEMORY
    re.setPattern("(\\d+\\.\\d+)\\s+memory\\s+used\\s+for\\s+hash\\s+table");
    QRegularExpressionMatch hashmemoryM = re.match(output);
    hashmemory = hashmemoryM.captured(1);

    //DFS MEMORY
    re.setPattern("(\\d+\\.\\d+)\\s+memory\\s+used\\s+for\\s+.FS\\s+stack");
    QRegularExpressionMatch DFSmemoryM = re.match(output);
    DFSmemory = DFSmemoryM.captured(1);

    //TOTAL MEMORY
    re.setPattern("(\\d+\\.\\d+)\\s+total\\s+actual\\s+memory\\s+usage");
    QRegularExpressionMatch totalmemoryM = re.match(output);
    totalmemory = totalmemoryM.captured(1);

}
