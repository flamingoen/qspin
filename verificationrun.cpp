#include "verificationrun.h"

VerificationRun::VerificationRun(QString _path, VerificationType _type, bool _fairness, QStringList _compileOptions, int _searchDepth, int _hashSize) {
    path = _path;
    type = _type;
    fairness = _fairness;
    compileOptions = _compileOptions;
    searchDepth = _searchDepth;
    hashSize = _hashSize;
}

VerificationRun::~VerificationRun() {
    process->deleteLater();
}

void VerificationRun::start(){
    process = new QProcess();
    currentOutput="";
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(runCompile()));
    setStatus("Verification: Creating model");
    process->start(SPIN,QStringList()<<"-a" << path.replace(" ","\\ "));
}

void VerificationRun::runCompile(){
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
    switch (type) {
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

void VerificationRun::setOutput() {
    QRegExp rx("error: max search depth too small");
    QString newOutput = process->readAllStandardOutput();
    if (newOutput.contains(rx)) {
        if (searchDepth != -1) searchDepth += (searchDepth/2);
        else searchDepth = 15000;
        currentOutput.append("max search depth too small: restarting with new depth = "+QString::number(searchDepth));
        runPan();
    } else {
        currentOutput.append(newOutput);
    }
}

void VerificationRun::setStatus(QString status) {
    currentStatus = status;
    emit statusChanged();
}

QString VerificationRun::readOutput(){
    QString str = currentOutput;
    currentOutput="";
    return str;
}

QString VerificationRun::readStatus(){
    return currentStatus;
}
