#ifndef SIMULATIONRUN_H
#define SIMULATIONRUN_H

#include <QMap>
#include <QStack>
#include <spinrun.h>
#include <QRegularExpression>

class SimulationRun : public SpinRun {
    Q_OBJECT
    Q_ENUMS(SimulationType)

public:
    enum SimulationType {Random, Interactive, Guided};
    SimulationRun(QString _path, SimulationType _type, int _depth);
    void goForward(int steps=1 );
    void goBackwards(int steps=1);
    SimulationType simulationType;

private:
    int depth;
    struct variable {
        int value;
        int process;
    };
    struct step {
        int line;
        int i_proc;
        int i_state;
        QString var;
        int change;
    };
    struct proc {
        int line;
        QString name;
    };

    step currentStep;
    QStack<step> statesBack;
    QStack<step> statesForward;
    QMap<int,proc> mapProcess;
    QMap<QString,variable> mapVariable;
    QString filename;


    void randomSimulation();
    void interactiveSimulation();
    void guidedSimulation();
    void parseSimulation(QString input);
    bool parseStep(QString _step);
    bool parseProc(QString _step);
    bool parseVar(QString _step);
    void setupProcess();

private slots:
    void finishedProcess();
    void readReadyProcess();

public slots:
    void start();
};

#endif // SIMULATIONRUN_H
