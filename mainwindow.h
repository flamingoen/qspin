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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStringList getRunOptions();
    QStringList getCompileOptions();
    void resetProcess();
    bool prepareRun(bool clearLog=true);
    VerificationRun::VerificationType verificationType();
    int searchDepth();
    int hashSize();

private slots:
    void loadFile();
    void runRandomSimulation();
    void runInteractiveSimulation();
    void runSubmitInteractiveSimulation();
    void runGuidedSimulation();
    void runVerify();
    void saveFile();
    void runProcessFinished();
    void runProcessReadReady();
    void terminateProcess();
    void fileCleanup();
    void checkSyntax();
    void checkSyntaxErrorHighlight();
    void verificationFinished();
    void verificationReadReady();
    void verificationStatusChange();
    void processVerificationOutput(QString output);
    void updateVerificationTab();
};

#endif // MAINWINDOW_H
