#ifndef VERIFICATIONRUN_H
#define VERIFICATIONRUN_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>

class VerificationRun : public QObject{
    Q_OBJECT
    Q_ENUMS(VerificationType)

    #ifdef _WIN32
        #define SPIN  "spin\\spin.exe"
        #define CCOMPILER "gcc"
    #else
        #define SPIN "spin"
        #define CCOMPILER "cc"
    #endif

public:

    enum VerificationType {Safety, Acceptance, Liveness};
    VerificationRun(QString _path, VerificationType _type, bool _fairness, QStringList _runOptions, int _searchDepth = -1, int _hashSize = -1);
    ~VerificationRun();
    QString readOutput();
    QString readStatus();

private:

    QProcess* process;
    QString path;
    QString currentOutput;
    QString finishedOutput;
    QString currentStatus;
    int type;
    bool fairness;
    QStringList compileOptions;
    int searchDepth;
    int hashSize;

    void setStatus(QString status);
    void setOutput();

signals:
    void readReady();
    void finished();
    void statusChanged();

private slots:
    void finishedVerification();
    void readReadyVerification();
    void runCompile();
    void runPan();
    void start();

};

#endif // VERIFICATIONRUN_H
