#include "spinrun.h"
#include "QObject"

//TODO add destructor

SpinRun::SpinRun(QString _path, QString _fileName, Type _type) {
    path = _path;
    fileName = _fileName;
    type = _type;
    filePath = path + QDir::separator() + fileName;
}

void SpinRun::setStatus(QString status) {
    if (currentStatus!="") currentStatus.append("\n");
    currentStatus.append(status);
    emit statusChanged(this);
}

QString SpinRun::readOutput(){
    QString str = currentOutput.append("\n");
    currentOutput="";
    return str;
}

QString SpinRun::readStatus(){
    QString status = currentStatus;
    currentStatus = "";
    return status;
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

QString SpinRun::listToString(QStringList list) {
    QString s = "";
    foreach(QString item , list) { s.append(item+" "); }
    return s;
}
