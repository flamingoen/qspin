#ifndef VERIFICATIONRUN_H
#define VERIFICATIONRUN_H

#include <spinrun.h>

#ifdef _WIN32
    #define PAN QString(path)+QDir::separator()+"pan.exe"
#else
    #define PAN "./pan"
#endif

class VerificationRun : public SpinRun {
    Q_OBJECT

public:
    enum VerificationType {Safety, Acceptance, Liveness};
    Q_ENUMS(VerificationType)
    VerificationRun(QString _spin, QString _compiler, QString _path, QString _fileName, VerificationType _type, bool _fairness,QString _ltl ,QStringList _compileOptions, int _searchDepth = -1, int _hashSize = -1, bool _autoDepth = true);
    ~VerificationRun();
    VerificationType verificationType;

    QString compiler;

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
    bool autoDepth;
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
