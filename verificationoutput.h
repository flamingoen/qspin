#ifndef VERIFICATIONOUTPUT_H
#define VERIFICATIONOUTPUT_H

#include <QRegularExpression>
#include <iostream>
class VerificationOutput
{
public:
    VerificationOutput();

    //STATESPACE SPECS
    QString spinVer ;
    QString eval ;
    QString partial;
    QString never;
    QString assertion;
    QString acceptanceType;
    QString acceptance;
    QString invalid;

    //STATESPACE PROP
    QString errors;
    QString depth;
    QString storedStates;
    QString matchedStates;
    QString transitions;
    QString atomic;
    QString statesize;
    QString hashconflict;
    QString hashsize;


    void processVerification(QString output);
};

#endif // VERIFICATIONOUTPUT_H
