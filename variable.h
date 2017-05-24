#ifndef VARIABLE_H
#define VARIABLE_H

#include <QObject>

class Variable : public QObject {
    Q_OBJECT

public:
    QString type, name, value, process, pid;
    QList<Variable> vars;
    int id;
    Variable(QString type, QString name, QString process = "global", QString pid = 0);
    void setValue(QString value);

private:
    static int lastId;
};

#endif // VARIABLE_H
