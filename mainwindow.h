#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <QSyntaxHighlighter>
#include "highlighter.h"
#include "codeeditor.h"

#include "verificationrun.h"
#include "simulationrun.h"

#include "ui_mainwindow.h"
#include "verificationoutput.h"
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
QString path;
QString filename;
QStatusBar *status;
CodeEditor *editor;
QTextEdit *outputLog;

QRadioButton *radioSafety;
QRadioButton *radioAcceptance;
QRadioButton *radioLiveness;
QRadioButton *radioColapse;
QRadioButton *radioDH4;
QRadioButton* radioInteractive;
QRadioButton* radioGuided;

QCheckBox *checkFair;
QCheckBox *checkHSize;
QCheckBox *checkOptDepth;

QSpinBox *spinBoxHSize;
QSpinBox *spinBoxSteps;
QSpinBox *spinBoxSDepth;
QComboBox *comboChoice;

QProcess *process;
SpinRun *spinRun;

QFile file;

// Verification tab
QLabel *spinVerLabel;
QLabel *evalLabel;
QLabel *partialLabel;
QLabel *neverLabel;
QLabel *assertionLabel;
QLabel *acceptanceLabel;
QLabel *invalidLabel;
QLabel *simulationTypeLabel;
QLabel *fileLabel;

QLabel * errorLabel;
QLabel * depthLabel;
QLabel * storedstatesLabel;
QLabel * matchedstatesLabel;
QLabel * transitionLabel;
QLabel * atomicLabel;
QLabel * statesizeLabel;
QLabel * hashconflictsLabel;
QLabel * hashsizeLabel;

QLabel * statememoryLabel;
QLabel * hashmemoryLabel;
QLabel * DFSmemoryLabel;
QLabel * totalmemoryLabel;

QLabel * timestampLabel;

VerificationOutput *verificationOutput;

bool newVerification = false;


    Ui::MainWindow *ui;
    QStringList getRunOptions();
    QStringList getCompileOptions();
    bool prepareRun(bool clearLog=true);
    void runProcess(SpinRun* run);

private slots:
    void loadFile();
    void saveFile();
    void runVerify();
    void runSimulation();
    void terminateProcess();
    void fileCleanup();
    void checkSyntax();
    void checkSyntaxErrorHighlight();
    void processFinished();
    void processReadReady();
    void processStatusChange();
    void processVerificationOutput(QString output);
    void updateVerificationTab();
    void simulationStepForward();
    void simulationStepBackwards();
};

#endif // MAINWINDOW_H
