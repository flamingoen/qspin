#ifndef VERIFICATIONOUTPUT_H
#define VERIFICATIONOUTPUT_H

#include <QRegularExpression>
#include <iostream>
#include <QDateTime>

class VerificationOutput
{
public:
    VerificationOutput();

    QString timestamp;

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

    // MEMORY USAGE
    QString statememory;
    QString hashmemory;
    QString DFSmemory;
    QString totalmemory;


    void processVerification(QString output);
};

#endif // VERIFICATIONOUTPUT_H
