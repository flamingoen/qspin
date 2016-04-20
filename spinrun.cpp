#include "spinrun.h"
#include "QObject"

SpinRun::SpinRun(QString _path, Type _type) {
    path = _path;
    type = _type;
}

void SpinRun::setStatus(QString status) {
    currentStatus = status;
    emit statusChanged();
}

QString SpinRun::readOutput(){
    QString str = currentOutput;
    currentOutput="";
    return str;
}

QString SpinRun::readStatus(){
    return currentStatus;
}
