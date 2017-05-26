#include "variable.h"

int Variable::lastId = 0;

Variable::Variable(QString type, QString name, QString process) {
    this->id = lastId++;
    this->type = type;
    this->name = name;
    this->process = process;
}

void Variable::setValue(QString value, int index) {
    if (this->value.size()>=index+1) this->value.replace(index,value);
    else this->value.insert(index,value);
}

void Variable::setValue(QList<QString> value) {
    this->value = value;
}


QString Variable::getValueString()   {
    if (value.size()>1) {
        QString returnVal = "{"+value[0];
        for (int i = 1 ; i<value.size() ; i++) {
            returnVal.append(","+value[i]);
        }
        return returnVal.append("}");
    }
    else if (value.size()==0) return "-";
    else return value[0];

}

QList<QString> Variable::getValue() { return value; }

QString Variable::getName()    { return name; }
int     Variable::getId()      { return id; }
