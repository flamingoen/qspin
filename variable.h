#ifndef VARIABLE_H
#define VARIABLE_H

#include <QObject>

class Variable : public QObject {
    Q_OBJECT

public:
    QList<Variable> vars;
    int id;
    Variable(QString type, QString name, QString process = "global");
    void setValue(QString value, int index = 0);
    void setValue(QList<QString> value);
    QString getValueString();
    QList<QString> getValue();
    QString getName();
    int getId();

private:
    QString type, name, process;
    QList<QString> value;
    static int lastId;
};

#endif // VARIABLE_H
