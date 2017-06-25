#include "simulationrun.h"
#include <QDebug>

SimulationRun::SimulationRun(QString _spin, QString _path, QString _fileName, SimulationType _type, int _depth) : SpinRun(_spin, _path , _fileName, Simulation){
    simulationType = _type;
    depth = _depth;
    statesBack.clear();
    statesForward.clear();
    currentStep.operation = "Initializing";
}

SimulationRun::~SimulationRun(){
    process->deleteLater();
}

void SimulationRun::terminateProcess(){
    SpinRun::terminateProcess();
    listChoises.clear();
}

void SimulationRun::start() {
    if (parseCode()) {
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
}

void SimulationRun::randomSimulation() {
    setStatus("Running random simulation with: " + QString(spin) + "-u" + QString::number(depth) + "-p -g -l" + "\""+fileName+"\"");
    setStatus("\""+path+ QDir::separator() + fileName+"\"");
    setupProcess();
    process->setWorkingDirectory(path);
    process->start(spin,QStringList() << "-u"+QString::number(depth) << "-p" << "-g" << "-l" << "\""+fileName+"\"");
}

void SimulationRun::interactiveSimulation() {
    setupProcess();
    process->setWorkingDirectory(path);
    connect(process, SIGNAL(readyReadStandardOutput()),this,SLOT(readReadyProcess()));
    while(!statesForward.isEmpty()) {
        goForward();
    }
    setStatus("Running interactive simulation with spin -g -l -p -i");
    process->start(spin,QStringList() << "-g" << "-l" << "-p" << "-i"  << "\""+fileName+"\"");
}

void SimulationRun::guidedSimulation() {
    QString trailPath = QDir::toNativeSeparators(path+ QDir::separator() + fileName) + ".trail";
    QDir dir(QDir::currentPath());
    dir.setNameFilters(QStringList() << "*.trail");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList()) {
        QFile::copy(dirFile, trailPath);
    }
    if (QFile::exists(trailPath)){
        setStatus("Running guided simulation with -t -g -l -p -r -s -X");
        setupProcess();
        process->setWorkingDirectory(path);
        process->start(spin,QStringList() << "-t" << "-g" << "-l" << "-p" << "-r" << "-s" << "-X" << "\""+fileName+"\"");
    } else emit processError("Cannot run simulation: Trail path not found");
}

void SimulationRun::setupProcess(){
    process = new QProcess(this);
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedProcess()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), process, SLOT(deleteLater()));
    connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(error()));
    step newStep;
    newStep.operation="Simulation";
    statesBack.push(currentStep);
    currentStep = newStep;
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
    emit finished(this);
}

void SimulationRun::readReadyProcess() {
    listChoises.clear();
    QString input = process->readAllStandardOutput();
    while (!statesForward.isEmpty()) {
        goForward();
    }
    parseSimulation(input);
    currentOutput.append(input);
    foreach(QString line , input.split("\n")){
        parseChoises(line);
    }
    while (!statesBack.isEmpty()) {
        goBackwards();
    }
    emit readReady(this);
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

bool SimulationRun::parseCode() {
    QFile file(path+ QDir::separator() + fileName);
    QString codeText = "";

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    setStatus("Failed to open file");
    } else {
        QTextStream in(&file);
        while(!in.atEnd()) {
            codeText.append(in.readLine()+"\n");
        }
        file.close();
    }
    std::string value = codeText.remove(QRegularExpression("\\/\\*(?:.|\\n)*?\\*\\/|\\/\\/.*?\\\n")).toStdString();
    std::string::const_iterator iter = value.begin();
    std::string::const_iterator end = value.end();
    std::vector<std::vector<std::string>> parseList;
    bool res = phrase_parse(iter,end,grammar,space,parseList);
    if ( res ) {
        for ( uint i=0 ; i<parseList.size() ; i++) {
            createVariables(parseList[i]);
        }
    } else {
        setStatus("QSpin could not parse file");
    }
    return res;
}

bool SimulationRun::parseStep(QString _step) {
    QRegularExpression reStep("\\B\\s*\\d+:\\s+proc\\s+(\\d+)\\s+\\((.*?):\\d+\\)\\s"+fileName+":(\\d+)\\s+\\(state\\s(\\d+)\\)\\s+\\[(.*?)\\]");
    QRegularExpressionMatch match = reStep.match(_step);
    bool matched = match.hasMatch();
    if (matched) {
        step newStep;
        proc _proc;
        newStep.i_proc = match.captured(1).toInt();
        if (!mapProcess.contains(newStep.i_proc)) {
            _proc.id = p_id; p_id++;;
            _proc.name = match.captured(2).append('['+match.captured(1)+']');
        } else {
            _proc = mapProcess[newStep.i_proc];
        }
        newStep.oldLine = _proc.line;
        _proc.line = match.captured(3).toInt();
        newStep.newLine = _proc.line;
        newStep.i_state = match.captured(4).toInt();
        newStep.operation = _proc.name+":\t"+match.captured(5);
        statesBack.push(currentStep);
        currentStep = newStep;
        mapProcess.insert(newStep.i_proc,_proc);
    }
    return matched;
}

bool SimulationRun::parseProc(QString _step) {
    QRegularExpression reCreateProc("creates proc\\s+(\\d+)\\s+\\((.*)?\\)");
    QRegularExpressionMatch match = reCreateProc.match(_step);
    bool matched = match.hasMatch();
    if (matched) {
        QString procName = match.captured(2);
        QString procPid = match.captured(1);
        QList<QString> varList = procTemplate[procName];
        for (int i = 0 ; i+1<varList.size() ; i+=2) {
            QString varName = procName+'['+procPid+"]:"+varList[i+1];
            mapVariables.insert(varName,new Variable(varList[i],varName,procName));
        }
    }
    return matched;
}

// Tries to find af variable in the step. returns true if found, false otherwise
bool SimulationRun::parseVar(QString _step) {
    QRegularExpression reVariable("\\t\\t([a-zA-Z0-9]+)(\\[(\\d)\\])?\\s+=\\s+(.*)");
    QRegularExpression reLocVar("\\t\\t([a-zA-Z0-9]+)\\((\\d+)\\):([a-zA-Z0-9]+)(\\[(\\d)\\])?\\s=\\s(.*)");
    QRegularExpressionMatch matchG = reVariable.match(_step);
    QRegularExpressionMatch matchL = reLocVar.match(_step);
    bool matched = matchG.hasMatch() || matchL.hasMatch();
    QString varName, varValue, varIndex;

    // match either global or local variable
    if (matchG.hasMatch()) {
        varName = matchG.captured(1);
        varIndex = matchG.captured(3);
        varValue = matchG.captured(4);
    } else if (matchL.hasMatch()) {
        varName = matchL.captured(1).append('[' + matchL.captured(2) + "]:" + matchL.captured(3));
        varIndex = matchL.captured(5);
        varValue = matchL.captured(6);
    }

    // Add change to the step and replace in the map
    if (matched) {
        Variable *var = mapVariables[varName];
        currentStep.oldValue = var->getValue();
        currentStep.var = varName;
        if (!varIndex.compare("")) var->setValue(varValue);
        else var->setValue(varValue,varIndex.toInt());
        currentStep.newValue = var->getValue();;
    }

    return matched;
}

void SimulationRun::parseChoises(QString _step) {
    QString procString = "proc\\s+(\\d+)\\s+\\(.*?\\)\\s+"+fileName+":\\d+\\s+\\(state\\s+\\d+\\)\\s";
    QRegularExpression reChoise("choice\\s+(\\d+):\\s+"+procString+"\\[(.*?)\\]");
    QRegularExpressionMatch match = reChoise.match(_step);
    if (match.hasMatch()) {
        choise _choise;
        _choise.number = match.captured(1);
        _choise._proc = mapProcess[match.captured(2).toInt()];
        _choise.operation = match.captured(3);
        listChoises.append(_choise);
    } else {
        QRegularExpression reChoise("choice\\s+(\\d+):\\s+(.*)");
        match = reChoise.match(_step);
        if (match.hasMatch()) {
            choise _choise;
            _choise.number = match.captured(1);
            _choise.operation = match.captured(2);
            listChoises.append(_choise);
        }
    }
}

void SimulationRun::createVariables(std::vector<std::string> list) {
    QString varProc = "global";
    QString varType;

    if (!list[0].compare("PROCTYPE")) {
        QString procName = QString::fromStdString(list[1]);
        QList<QString> varList;
        for ( uint j=2 ; j<list.size() ; j++) {
            if (!list[j].compare("INLINE")) {
                 QString funcName = QString::fromStdString(list[j+1]);
                 QList<QString> funcVars = inlineTemplate[funcName];
                 varList.append(funcVars);
                 j++;
            } else varList << QString::fromStdString(list[j]);
        }
        procTemplate.insert(procName,varList);
    } else if (!list[0].compare("INLINE")) {
        inlineTemplate.insert(QString::fromStdString(list[1]),makeInlineVars(list));
    } else {
        for ( uint j = 0 ; j<list.size() ; j++) {
            QString tmp = QString::fromStdString(list[j]);
            if (types.contains(tmp)) varType = tmp;
            else mapVariables.insert( tmp , new Variable( varType , tmp , varProc));
        }
    }
}

QList<QString> SimulationRun::makeInlineVars(std::vector<std::__cxx11::string> list) {
    QList<QString> varList;
    for ( uint j=2 ; j<list.size() ; j++) {
        if (!list[j].compare("INLINE")) {
            j++;
            varList.append(inlineTemplate[QString::fromStdString(list[j])]);
        } else
            varList << QString::fromStdString(list[j]);
    }
    return varList;
}

void SimulationRun::goForward(int steps) {
    for (int i=0 ; i<steps ; i++) {
        if (!statesForward.isEmpty()) {
            statesBack.push(currentStep);
            currentStep = statesForward.pop();
            if (currentStep.var!="-") {
                mapVariables[currentStep.var]->setValue(currentStep.newValue);
            }
            if (currentStep.i_proc!=-1) {
                mapProcess[currentStep.i_proc].line = currentStep.newLine;
            }
        }
    }
}

void SimulationRun::goBackwards(int steps) {
    for (int i=0 ; i<steps ; i++) {
        if (!statesBack.isEmpty()) {
            if (currentStep.var!="-") {
                mapVariables[currentStep.var]->setValue(currentStep.oldValue);
            }
            if (currentStep.i_proc!=-1) {
                mapProcess[currentStep.i_proc].line = currentStep.oldLine;
            }
            statesForward.push(currentStep);
            currentStep = statesBack.pop();
        }
    }
}

QList<Variable*> SimulationRun::getVariables() {
    QList<Variable*> list;
    foreach(QString key , mapVariables.keys()) {
        list << mapVariables[key];
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
    return currentStep.var!="-";
}

int SimulationRun::getCurrentVarId() {
    return mapVariables[currentStep.var]->id;
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

int SimulationRun::getCurrentIndex() {
    return statesBack.length();
}

QList<SimulationRun::choise> SimulationRun::getChoises() {
    return listChoises;
}

QStringList SimulationRun::getOperations() {
    QStringList operations;
    QStack<step> tempStatesBack;
    QStack<step> tempStatesForward;
    step _step;
    // Get operations from the stacks
    while (!statesBack.isEmpty()) {
        _step = statesBack.pop();
        operations.append(_step.operation);
        tempStatesBack.push(_step);
    }
    operations.append(currentStep.operation);
    while (!statesForward.isEmpty()) {
        _step = statesForward.pop();
        operations.append(_step.operation);
        tempStatesForward.push(_step);
    }
    // Re-create stacks
    while (!tempStatesBack.isEmpty()) {
        statesBack.push(tempStatesBack.pop());
    }
    while (!tempStatesForward.isEmpty()) {
        statesForward.push(tempStatesForward.pop());
    }
    return operations;
}

bool SimulationRun::canGoForward() {
    return !statesForward.isEmpty();
}

bool SimulationRun::canGoBackwards() {
    return !statesBack.isEmpty();
}

void SimulationRun::commitChoise(QModelIndex index) {
    QString cmd = listChoises[index.row()].number+"\n";
    process->write(cmd.toLatin1().data());
}
