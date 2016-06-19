#ifndef SYNTAXRUN_H
#define SYNTAXRUN_H

#include <spinrun.h>

class SyntaxRun : public SpinRun{
    Q_OBJECT

public:
    SyntaxRun(QString _path, QString _ltl);
    ~SyntaxRun();
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
    void noErrors();
    void hasErrors();

private slots:
    void checkSyntax();

public slots:
    void start();
};


#endif // SYNTAXRUN_H
