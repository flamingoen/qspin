#include "simulationrun.h"

SimulationRun::SimulationRun(QString _path, int _type, int _depth) : SpinRun(_path , Simulation){
    simulationType = _type;
    depth = _depth;
    QRegExp rx("/((([a-z]|[A-Z]|\\d)+).pml)");
    rx.indexIn(path);
    filename = rx.cap(1);
}

void SimulationRun::start() {
    switch (simulationType) {
    case Random:
        randomSimulation();
        break;
    case Interactive:
        interactiveSimulation();
        break;
    case Guided:
        guidedSimulation();
        break;
    }
}

void SimulationRun::randomSimulation() {
    setStatus("Running random simulation");
    setupProcess();
    process->start(SPIN,QStringList() << "-u200" << "-p" << "-g" << "-l" << path);
}

void SimulationRun::interactiveSimulation() {
    setStatus("Running interactive simulation");
    setupProcess();
    process->start(SPIN,QStringList() << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "-i"  << path);
}

void SimulationRun::guidedSimulation() {
    QString trailPath = QDir::toNativeSeparators(path) + ".trail";
    QFile::copy(filename+".trail", trailPath);
    setStatus("Running guided simulation");
    setupProcess();
    process->start(SPIN,QStringList() << "-t" << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "-u250" << path.replace(" ","\\ "));
}

void SimulationRun::setupProcess(){
    process = new QProcess();
    connect(process, SIGNAL(readyReadStandardOutput()),this,SLOT(readReadyProcess()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedProcess()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), process, SLOT(deleteLater()));
}

void SimulationRun::finishedProcess() {
    QString input = process->readAllStandardOutput();
    parseSimulation(input);
    currentOutput.append(input);
    while (!statesBack.isEmpty()) {
        statesForward.push(statesBack.pop());
    }
    if (!statesForward.isEmpty()) currentStep = statesForward.pop();
    emit finished();
}

void SimulationRun::readReadyProcess() {
    QString input = process->readAllStandardOutput();
    parseSimulation(input);
    currentOutput.append(input);
    emit readReady();
}

void SimulationRun::parseSimulation(QString input) {
    foreach(QString _step, input.split("\n")) {
        if (!parseStep(_step)){
            if (!parseProc(_step)) {
                parseVar(_step);
            }
        }
    }
}

bool SimulationRun::parseStep(QString _step) {
    QRegularExpression reStep("proc\\s+(\\d+)\\s+\\(.*?\\)\\s"+path+":(\\d+)\\s+\\(state\\s(\\d+)");
    QRegularExpressionMatch match = reStep.match(_step);
    bool matched = match.hasMatch();
    if (matched) {
        step newStep;
        newStep.i_proc = match.captured(1).toInt();
        newStep.line = match.captured(2).toInt();
        newStep.i_state = match.captured(3).toInt();
        statesBack.push(newStep);
        mapProcess[newStep.i_proc].line = newStep.line;
    }
    return matched;
}

bool SimulationRun::parseProc(QString _step) {
    QRegularExpression reCreateProc("creates proc\\s+(\\d+)\\s+\\((.*)?\\)");
    QRegularExpressionMatch match = reCreateProc.match(_step);
    bool matched = match.hasMatch();
    if (matched) {
        proc newProc;
        newProc.name = match.captured(2);
        newProc.line = 0;
        mapProcess.insert(match.captured(1).toInt(),newProc);
    }
    return matched;
}

bool SimulationRun::parseVar(QString _step) {
    QRegularExpression reVariable("\\t\\t([a-zA-Z0-9]+)\\s+=\\s+(\\d+)"); //note: two tabs for variables, spaces for print stements.
    QRegularExpressionMatch match = reVariable.match(_step);
    bool matched = match.hasMatch();
    if (match.hasMatch()) {
        variable newVar;
        QString varName = match.captured(1);
        newVar.value = match.captured(2).toInt();
        step _step = statesBack.pop();
        if (mapVariable.contains(varName)) {
            variable oldVar = mapVariable[varName];
            _step.change = oldVar.value-newVar.value;
        } else {
            _step.change = newVar.value;
        }
        _step.var = varName;
        mapVariable.insert(varName,newVar);
        statesBack.push(_step);
    }
    return matched;
}

void SimulationRun::goForward(int steps) {
    for (int i=0 ; i<steps ; i++) {
        statesBack.push(currentStep);
        currentStep = statesForward.pop();
    }
}

void SimulationRun::goBackwards(int steps) {
    for (int i=0 ; i<steps ; i++) {
        statesForward.push(currentStep);
        currentStep = statesBack.pop();
    }
}

//void MainWindow::runSubmitInteractiveSimulation() {
//    if (!process->state() == 0) {
//        QString cmd = comboChoice->currentText() + "\n";
//        process->write(cmd.toLatin1().data());
//    } else { process->terminate(); }
//}
