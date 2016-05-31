#include "verificationrun.h"

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
     process->start(SPIN,QStringList() << "-a" << tempPath.replace(" ","\\ "));
    }
    else process->start(SPIN,QStringList() << "-a" << path.replace(" ","\\ "));
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
    setStatus("Varification: Compiling pan.c");
    process->start(CCOMPILER, compileOptions << "pan.c");
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

    setStatus("Varification: Running verification");
    process->start("./pan", runOptions);
}

void VerificationRun::finishedVerification() {
    currentOutput.append(process->readAllStandardOutput());
    emit finished();
}

void VerificationRun::readReadyVerification() {
    QRegExp rx("error: max search depth too small");
    QString newOutput = process->readAllStandardOutput();
    if (newOutput.contains(rx)) {
        if (searchDepth != -1) searchDepth += (searchDepth/2);
        else searchDepth = 15000;
        currentOutput.append("max search depth too small: restarting with new depth = "+QString::number(searchDepth));
        process->disconnect();
        runPan();
    } else {
        currentOutput.append(newOutput);
    }
    emit readReady();
}
