#ifndef VERIFICATIONRUN_H
#define VERIFICATIONRUN_H

#include <spinrun.h>

class VerificationRun : public SpinRun {
    Q_OBJECT

public:
    enum VerificationType {Safety, Acceptance, Liveness};
    Q_ENUMS(VerificationType)
    VerificationRun(QString _path, VerificationType _type, bool _fairness,QString _ltl ,QStringList _compileOptions, int _searchDepth = -1, int _hashSize = -1);
    ~VerificationRun();
    VerificationType verificationType;

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

    //functions
    QString getAcceptanceType();

private:
    QString finishedOutput;
    bool fairness;
    QStringList compileOptions;
    QString ltl;
    int searchDepth;
    int nFair = 2;
    int hashSize;
    QString tempPath;
    void processVerification(QString output);

private slots:
    QString createTempPml();
    void finishedVerification();
    void readReadyVerification();
    void runCompile();
    void runPan();

public slots:
    void start();

};

#endif // VERIFICATIONRUN_H
