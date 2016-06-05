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
