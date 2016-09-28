#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <QSyntaxHighlighter>
#include "highlighter.h"
#include "codeeditor.h"

#include "verificationrun.h"
#include "simulationrun.h"
#include "syntaxrun.h"

#include "ui_mainwindow.h"

#ifdef __linux__
 #define OS "linux"
#else
 #define OS "nonLinux"
#endif

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void runSyntax(QString ltl);

private:
    QString ltl = "";
    QString path;
    QString LTLpath;
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
    QCheckBox *checkSaveDepth;

    QSpinBox *spinBoxHSize;
    QSpinBox *spinBoxSteps;
    QSpinBox *spinBoxSDepth;

    QComboBox *comboBoxSearch;

    QListWidget *simulationSteps;
    QListWidget *simulationSteps_I;
    QListWidget *listChoises;

    QProcess *process;
    SyntaxRun *syntaxRun;
    VerificationRun *verificationRun;
    SimulationRun *simulationRun;
    SimulationRun *interactiveRun;

    QFile file;
	QFile LTLfile; // Could be local
	int selectedLtl;

    SimulationRun::SimulationType simType;
    VerificationRun::VerificationType verType;
    QListWidget* ltlList;
    QStringList compileOpts;
    QStringList runOpts;
    QTableWidget* variableTable;
    QTableWidget* processTable;
    QTableWidget* variableTable_I;
    QTableWidget* processTable_I;
    QAction* actionCheckSyntax;

    QPushButton* newltlButton;
    QPushButton* deleteltlButton;
    QPushButton* buttonForwardSim;
    QPushButton* buttonBackSim;
    QPushButton* buttonVerify;
    QPushButton* buttonRandomSim;
    QPushButton* buttonInteractiveSim;

    QGroupBox* groupBox_log;


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
    QLabel *fileLabel_I;
    QLabel *CycleTypeLabel;

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

    bool newVerification = false;

    QThread *thread;

    Ui::MainWindow *ui;
    QStringList getRunOptions();
    QStringList getCompileOptions();
    QString getLtl();
    bool prepareRun(bool clearLog=true);
    QThread* connectProcess(SpinRun* run);
    void clearVerificationTab();
    void clearSimulationTab();
    void clearInteractiveTab();
    void updateStepList(QListWidget *stepList ,SimulationRun *run);
    void updateSimulationTab(SimulationRun *run, QTableWidget *variableTable, QTableWidget *processTable, QListWidget *stepList);
    void populateSimulationLists(SimulationRun* run, QTableWidget* variableTable, QTableWidget* processTable, QListWidget *stepList);
    int hashSize();
    void saveLtlFile();
    void iconFallback();
    bool saveWarning();

signals:
    void closeProcess();

private slots:
    void loadFile();
    void loadLtlFile();
    void showAbout();
    void saveFile();
    void saveFileAs();
    void newFile();
    void runVerify();
    void runSimulation();
    void terminateProcess();
    void fileCleanup();
    void runCheckSyntax();
    void processFinished(SpinRun* run);
    void processReadReady(SpinRun* run);
    void processStatusChange(SpinRun* run);
    void displayErrors();
    void updateVerificationTab();
    void createSimulationTab();
    void createInteractiveTab();
    void simulationStepForward();
    void simulationStepBackwards();
    void verify();
    void simulation();
    void simulationStepClicked();
    void runInteractive();
    void listChoiseActivated(QModelIndex index);
    void newLtl();
    void deleteLtl();
    void interactiveStepClicked();
    void interactive();
    void enableRunButtons();
    void disableRunButtons();
    void processError(QString);
    void showHideLog(bool show);
    void exit();
};

#endif // MAINWINDOW_H
