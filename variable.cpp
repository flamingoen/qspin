#include "variable.h"

Variable::Variable(QString type, QString name, QString process) {
    this->type = type;
    this->name = name;
    this->process = process;
}

void Variable::setValue(QString value){
    this->value = value;
}
