#ifndef SIMULATIONRUN_H
#define SIMULATIONRUN_H

#include <QMap>
#include <QStack>
#include <spinrun.h>
#include <QRegularExpression>
#include <QList>
#include <QTableWidgetItem>

class SimulationRun : public SpinRun {
    Q_OBJECT
    Q_ENUMS(SimulationType)

public:
    enum SimulationType {Random, Interactive, Guided};
    SimulationRun(QString _path, SimulationType _type, int _depth);
    void goForward(int steps=1 );
    void goBackwards(int steps=1);
    enum SimulationType simulationType;
    int v_id = 0;
    int p_id = 0;

    struct variable {
        int id;
        QString name;
        QString type;
        QString value;
        int process;
    };
    struct step {
        int line;
        int i_proc;
        int i_state;
        QString var = "";
        QString value;
        QString operation;
    };
    struct proc {
        int id;
        int line = -1;
        QString name;
    };

    struct choise {
        QString number;
        proc _proc;
        QString operation;
    };

    QList<variable> getVariables();
    QList<proc> getProcs();
    step getCurrentStep();
    bool currentStepChangeVariable();
    int getCurrentVarId();
    QString getCurrentVarValue();
    int getCurrentProcId();
    int getCurrentProcLine();
    QStringList getOperations();
    int getCurrentIndex();
    bool canGoForward();
    bool canGoBackwards();
    QList<choise> getChoises();
    QString getCurrentVarName();
    void commitChoise(choise _choise);

private:
    int depth;
    step currentStep;
    QStack<step> statesBack;
    QStack<step> statesForward;
    QMap<int,proc> mapProcess;
    QMap<QString,variable> mapVariable;
    QList<choise> listChoises;
    QString filename;


    void randomSimulation();
    void interactiveSimulation();
    void guidedSimulation();
    void parseSimulation(QString input);
    bool parseStep(QString _step);
    bool parseProc(QString _step);
    bool parseVar(QString _step);
    void parseCode();
    void setupProcess();
    void parseChoises(QString _step);

private slots:
    void finishedProcess();
    void readReadyProcess();

public slots:
    void start();
};

#endif // SIMULATIONRUN_H
