#ifndef SPINRUN_H
#define SPINRUN_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>

#ifdef _WIN32
    #define SPIN  "spin\\spin.exe"
    #define CCOMPILER "gcc"
#else
    #define SPIN "spin"
    #define CCOMPILER "cc"
#endif

class SpinRun : public QObject{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type {Simulation,Verification,nonSpecified};
    SpinRun(QString _path, Type _type);
    QString readOutput();
    QString readStatus();
    void setStatus(QString status);
    Type type = nonSpecified;

protected:
    QProcess* process;
    QString path;
    QString currentOutput = "";
    QString finishedOutput;
    QString currentStatus;

signals:
    void readReady();
    void finished();
    void statusChanged();

public slots:
    virtual void start() = 0;
};

#endif // SPINRUN_H
