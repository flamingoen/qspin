#ifndef RUNPROCESS_H
#define RUNPROCESS_H

#include <QProcess>
#include <stdlib.h>
#include <iostream>
#include <QDebug>

class runProcess : public QObject {

    Q_OBJECT

public:
    runProcess();
    void terminate();
    void runAndWait(QString cmd, QStringList opt);
    QString runGetOutput(QString cmd, QStringList opt);
    QString runInputGetOutput(QString cmd);
    QString runGetOutputWaitForInput(QString cmd, QStringList opt);
    bool finished();
    void reset();

private:
    QProcess *process;
    QString output;

private slots:
    void setOutput();
};

#endif // RUNPROCESS_H
