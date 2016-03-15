#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <stdlib.h>
#include <sstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStringList getRunOptions();
    QStringList getCompileOptions();
    void resetProcess();

private slots:
    void loadFile();
    void runRandomSimulation();
    void runInteractiveSimulation();
    void runSubmitInteractiveSimulation();
    void runGuidedSimulation();
    void runVerify();
    void saveFile();
    void runProcessFinished();
    void runCompile();
    void runPan();
};

#endif // MAINWINDOW_H
