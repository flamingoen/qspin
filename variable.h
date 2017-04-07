#ifndef VARIABLE_H
#define VARIABLE_H

#include <QObject>

class Variable : public QObject {
    Q_OBJECT
    QString type, name, value, process;

public:
    Variable(QString type, QString name, QString process = "global");
    void setValue(QString value);
};

#endif // VARIABLE_H
