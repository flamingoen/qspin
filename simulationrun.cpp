#include "simulationrun.h"
#include <QDebug>

/* TODOLIST FOR SIMULATION
 * TODO: Implement seed
 * TODO: Remove created processes from variables and place in procs with correct name
 * TODO: Interactive simulation
 * TODO: Implement missing types (Array, chan, utype)
 * TODO: Lock editor while simulation is being made
 * TODO: Show what happens in each step
 * TODO: Remove redundant steps
 * TODO: Parse print statements
 * */

SimulationRun::SimulationRun(QString _path, SimulationType _type, int _depth) : SpinRun(_path , Simulation){
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
    QFile::copy(filename+".qspin.trail", trailPath);
    setStatus("Running guided simulation");
    setupProcess();
    process->start(SPIN,QStringList() << "-t" << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "-u250" << path.replace(" ","\\ "));
}

void SimulationRun::setupProcess(){
    process = new QProcess();
    connect(process, SIGNAL(readyReadStandardOutput()),this,SLOT(readReadyProcess()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedProcess()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), process, SLOT(deleteLater()));
    parseCode();
}

void SimulationRun::finishedProcess() {
    QString input = process->readAllStandardOutput();
    parseSimulation(input);
    currentOutput.append(input);
    while (!statesBack.isEmpty()) {
        goBackwards();
    }
    foreach(int key, mapProcess.keys()) {
        mapProcess[key].line = -1;
    }

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

void SimulationRun::parseCode() {
    QFile file(path);
    QString codeText = "";
    QStringList lines;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    setStatus("Failed to open file");
    } else {
        QTextStream in(&file);
        while(!in.atEnd()) {
            codeText.append(in.readLine()+"\n");
        }
        file.close();
    }
    qDebug() << codeText;
    codeText.remove(QRegularExpression("\\/\\*(?:.|\\n)*?\\*\\/|\\/\\/.*?\\\n")); //Removes all comments
    lines = codeText.split(QRegularExpression(";|\\n"),QString::SkipEmptyParts);
    QRegularExpression reVar("(byte|bool|int|pid|short|mtype)\\s+(.*?)\\s+=\\s+(.*)");
    QRegularExpressionMatch match;
    foreach(QString line, lines) {
        match = reVar.match(line);
        if (match.hasMatch()) {
            QStringList parsedLine = line.split(QRegularExpression("\\s+|,|="),QString::SkipEmptyParts);
            QString varType = parsedLine[0];
            for (int i=1 ; i<parsedLine.length() ; i++) {
                variable var;
                var.name = parsedLine[i];
                var.type = varType;
                i++; //incriment to get next value
                var.value = parsedLine[i];
                if (varType=="bool") {
                    if (var.value=="true") var.value = "1";
                    else var.value = "0";
                }
                var.id = v_id; v_id++;
                mapVariable.insert(var.name,var);
                step _step;
                _step.var = var.name;
                _step.value = var.value;
                statesBack.push(_step);
            }
        }
    }
}

bool SimulationRun::parseStep(QString _step) {
    QRegularExpression reStep("proc\\s+(\\d+)\\s+\\((.*?):\\d+\\)\\s"+path+":(\\d+)\\s+\\(state\\s(\\d+)");
    QRegularExpressionMatch match = reStep.match(_step);
    bool matched = match.hasMatch();
    if (matched) {
        step newStep;
        newStep.i_proc = match.captured(1).toInt();
        if (!mapProcess.contains(newStep.i_proc)) {
            proc newProc;
            newProc.id = p_id; p_id++;;
            newProc.name = match.captured(2).append('['+match.captured(1)+']');
            mapProcess.insert(newStep.i_proc,newProc);
        }
        newStep.line = match.captured(3).toInt();
        newStep.i_state = match.captured(4).toInt();
        statesBack.push(newStep);
    }
    return matched;
}

// As it is done in parseStep, this function is no longer needed
bool SimulationRun::parseProc(QString _step) {
    QRegularExpression reCreateProc("creates proc\\s+(\\d+)\\s+\\((.*)?\\)");
    QRegularExpressionMatch match = reCreateProc.match(_step);
    bool matched = match.hasMatch();
    if (matched) {
        proc newProc;
        newProc.name = match.captured(2).append('['+match.captured(1)+']');
        newProc.line = 0;
        newProc.id = p_id; p_id++;
        mapProcess.insert(match.captured(1).toInt(),newProc);
    }
    return matched;
}

// Tries to find af variable in the step. returns true if found, false otherwise
bool SimulationRun::parseVar(QString _step) {
    QRegularExpression reVariable("\\t\\t([a-zA-Z0-9]+)\\s+=\\s+(.*)"); //note: two tabs for variables, spaces for print stements.
    QRegularExpression reLocVar("\\t\\t([a-zA-Z0-9]+)\\((\\d+)\\):([a-zA-Z0-9]+)\\s=\\s(.*)");
    QRegularExpressionMatch matchG = reVariable.match(_step);
    QRegularExpressionMatch matchL = reLocVar.match(_step);
    bool matched = true;
    variable newVar;
    // match either global or local variable
    if (matchG.hasMatch()) {
        newVar.name = matchG.captured(1);
        newVar.value = matchG.captured(2);
    } else if (matchL.hasMatch()) {
        newVar.name = matchL.captured(1).append('[' + matchL.captured(2) + "]:" + matchL.captured(3));
        newVar.value = matchL.captured(4);
    } else matched = false;

    // Add change to the step and replace in the map
    if (matched) {
        step _step = statesBack.pop();
        if (mapVariable.contains(newVar.name)) {
            variable oldVar = mapVariable[newVar.name];
            _step.value = newVar.value;
            newVar.id = oldVar.id;
        } else {
            newVar.id = v_id; v_id++;
            _step.value = newVar.value;
        }
        _step.var = newVar.name;
        mapVariable.insert(newVar.name,newVar);
        statesBack.push(_step);
    }
    return matched;
}

void SimulationRun::goForward(int steps) {
    for (int i=0 ; i<steps ; i++) {
        if (!statesForward.isEmpty()) {
            statesBack.push(currentStep);
            currentStep = statesForward.pop();
            if (currentStep.var!="") {
                mapVariable[currentStep.var].value = currentStep.value;
            }
            mapProcess[currentStep.i_proc].line = currentStep.line;
        }
    }
}

void SimulationRun::goBackwards(int steps) {
    for (int i=0 ; i<steps ; i++) {
        if (!statesBack.isEmpty()) {
            statesForward.push(currentStep);
            currentStep = statesBack.pop();
            if (currentStep.var!="") {
                mapVariable[currentStep.var].value = currentStep.value;
            }
            mapProcess[currentStep.i_proc].line = currentStep.line;
        }
    }
}

QList<SimulationRun::variable> SimulationRun::getVariables() {
    QList<variable> list;
    foreach(QString key , mapVariable.keys()) {
        list << mapVariable[key];
    }
    return list;
}

QList<SimulationRun::proc> SimulationRun::getProcs() {
    QList<proc> list;
    foreach(int key , mapProcess.keys()) {
        list << mapProcess[key];
    }
    return list;
}

SimulationRun::step SimulationRun::getCurrentStep() {
    return currentStep;
}

bool SimulationRun::currentStepChangeVariable() {
    return currentStep.var!="";
}

int SimulationRun::getCurrentVarId() {
    return mapVariable[currentStep.var].id;
}

QString SimulationRun::getCurrentVarValue() {
    return mapVariable[currentStep.var].value;
}

QString SimulationRun::getCurrentVarName() {
    return currentStep.var;
}

int SimulationRun::getCurrentProcId() {
    return currentStep.i_proc;
}

int SimulationRun::getCurrentProcLine() {
    return mapProcess[currentStep.i_proc].line;
}

bool SimulationRun::canGoForward() {
    return !statesForward.isEmpty();
}

bool SimulationRun::canGoBackwards() {
    return !statesBack.isEmpty();
}

//void MainWindow::runSubmitInteractiveSimulation() {
//    if (!process->state() == 0) {
//        QString cmd = comboChoice->currentText() + "\n";
//        process->write(cmd.toLatin1().data());
//    } else { process->terminate(); }
//}
