#ifndef VERIFICATIONRUN_H
#define VERIFICATIONRUN_H

#include <spinrun.h>

class VerificationRun : public SpinRun {
    Q_OBJECT
    Q_ENUMS(VerificationType)

public:
    enum VerificationType {Safety, Acceptance, Liveness};
    VerificationRun(QString _path, VerificationType _type, bool _fairness ,QStringList _runOptions, int _searchDepth = -1, int _hashSize = -1);
    ~VerificationRun();
    VerificationType verificationType;

private:
    QString finishedOutput;
    bool fairness;
    QStringList compileOptions;
    QStringList ltlOptions;
    int searchDepth;
    int hashSize;

private slots:
    void finishedVerification();
    void readReadyVerification();
    void runCompile();
    void runPan();

public slots:
    void start();

};

#endif // VERIFICATIONRUN_H
