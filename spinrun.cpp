#include "spinrun.h"
#include "QObject"

//TODO add destructor

SpinRun::SpinRun(QString _path, Type _type) {
    path = _path;
    type = _type;
}

void SpinRun::setStatus(QString status) {
    currentStatus = status;
    emit statusChanged(this);
}

QString SpinRun::readOutput(){
    QString str = currentOutput.append("\n");
    currentOutput="";
    return str;
}

QString SpinRun::readStatus(){
    return currentStatus;
}

void SpinRun::terminateProcess(){
    if (process!=NULL) {
        process->close();
        setStatus("Process killed");
    }
}

void SpinRun::error() {
    QString errorMsg = "";
    switch (process->error()) {
    case QProcess::FailedToStart:
        errorMsg = "Process failed to start, check SPIN path or installation";
        break;
    case QProcess::Crashed:
        errorMsg = "Process crashed unexpected";
        break;
    case QProcess::Timedout:
        errorMsg = "Process stopped due to a time out";
        break;
    default:
        errorMsg = "Unexpected error while running process";
        break;
    }
    emit processError(errorMsg);
    terminateProcess();
}
