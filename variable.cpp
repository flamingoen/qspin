#include "variable.h"

int Variable::lastId = 0;

Variable::Variable(QString type, QString name, QString process, QString pid) {
    this->pid = pid;
    this->id = lastId++;
    this->type = type;
    this->name = name;
    this->process = process;
}

void Variable::setValue(QString value){
    this->value = value;
}
