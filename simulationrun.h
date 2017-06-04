#ifndef SIMULATIONRUN_H
#define SIMULATIONRUN_H

#include <QMap>
#include <QStack>
#include <spinrun.h>
#include <QRegularExpression>
#include <QList>
#include <QTableWidgetItem>
#include <QPair>
#include <variable.h>

#include "parser.cpp"

using boost::spirit::standard::space;
typedef parser::p_grammar<std::string::const_iterator> p_grammar;
class SimulationRun : public SpinRun {
    Q_OBJECT
    Q_ENUMS(SimulationType)

public:
    enum SimulationType {Random, Interactive, Guided};
    SimulationRun(QString _spin, QString _path, QString _fileName, SimulationType _type, int _depth);
    ~SimulationRun();
    void goForward(int steps=1 );
    void goBackwards(int steps=1);
    enum SimulationType simulationType;
    int v_id = 0;
    int p_id = 0;
    bool firstRun = true;

    struct step {
        int oldLine = -1;
        int newLine = -1;
        int i_proc = -1;
        int i_state = -1 ;
        QString var = "-";
        QList<QString> newValue;
        QList<QString> oldValue;
        QString operation = "-";
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

    QList<Variable*> getVariables();
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


private:
    int depth;
    step currentStep;// = {-1,-1,-1,"-","-","-","Initializing"};
    QStringList types = {"BIT", "BOOL", "BYTE", "PID", "SHORT", "INT", "MTYPE", "CHAN"};
    QStack<step> statesBack;
    QStack<step> statesForward;
    QMap<int,proc> mapProcess;
    QMap<QString,Variable *> mapVariables;
    QList<choise> listChoises;
    QString filename;
    QMap<QString,QList<QString>> procTemplate,inlineTemplate;


    void randomSimulation();
    void interactiveSimulation();
    void guidedSimulation();
    void parseSimulation(QString input);
    bool parseStep(QString _step);
    bool parseProc(QString _step);
    bool parseVar(QString _step);
    bool parseCode();
    void createVariables(std::vector<std::string>);
    void setupProcess();
    void parseChoises(QString _step);
    QList<QString> makeInlineVars(std::vector<std::string>);
    p_grammar grammar;

private slots:
    void finishedProcess();
    void readReadyProcess();

public slots:
    void start();
    void commitChoise(QModelIndex index);
    void terminateProcess();
};

#endif // SIMULATIONRUN_H
