#ifndef SYNTAXRUN_H
#define SYNTAXRUN_H

#include <spinrun.h>

class syntaxRun : public SpinRun{
    Q_OBJECT

public:
    syntaxRun(QString _path, QString _ltl, QString _callType);
    QStringList lineNoList;
    QStringList errorList;
    int errors;


private:
    QString path;
    QString ltl;
    QString createTempPml();
    QString tempPath;
    QString callType;

signals:
    void finishedNoErrors();

private slots:
    void checkSyntax();

public slots:
    void start();
};


#endif // SYNTAXRUN_H
