#ifndef SPINRUN_H
#define SPINRUN_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>

class SpinRun : public QObject{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type {Simulation,Verification,Syntax,nonSpecified};
    SpinRun(QString _spin, QString _path, QString _fileName, Type _type);
    QString readOutput();
    QString readStatus();
    void setStatus(QString status);
    QString listToString(QStringList list);
    QString spin;
    Type type = nonSpecified;

protected:
    QString path;
    QString fileName;
    QString filePath;
    QString currentOutput = "";
    QString finishedOutput;
    QString currentStatus;
    QProcess* process = NULL;

signals:
    void readReady(SpinRun* run);
    void finished(SpinRun* run);
    void statusChanged(SpinRun* run);
    void processError(QString errorMsg);

public slots:
    virtual void start() = 0;
    void terminateProcess();
    void error();
};

#endif // SPINRUN_H
