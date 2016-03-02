#include "runprocess.h"

QString output;

runProcess::runProcess() {
    process = new QProcess();
    QObject::connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(setOutput()));
}

QString runProcess::runGetOutput(QString cmd, QStringList opt) {
    output = "";
    process->start(cmd,opt);
    process->waitForFinished();
    return output;
}

void runProcess::runAndWait(QString cmd, QStringList opt) {
    process->start(cmd,opt);
    process->waitForFinished();
}

QString runProcess::runGetOutputWaitForInput(QString cmd, QStringList opt) {
    output="";
    process->start(cmd,opt);
    process->waitForReadyRead();
    return output;
}

QString runProcess::runInputGetOutput(QString cmd) {
    QString in = cmd+"\n";
    output += "\n";
    process->write(in.toLatin1().data());
    process->waitForReadyRead();
    return output;
}

bool runProcess::finished() {
    return process->state() == 0;
}

void runProcess::terminate() {
    process->kill();
    process->waitForFinished();
}

void runProcess::setOutput(){
    output += process->readAllStandardOutput();
    std::cout << output.toStdString();
}
