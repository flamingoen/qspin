#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <stdlib.h>
#include <sstream>
#include <runprocess.h>

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

private slots:
    void loadFile();
    void runRandomSimulation();
    void runInteractiveSimulation();
    void runSubmitInteractiveSimulation();
    void runGuidedSimulation();
    void runVerify();
    void saveFile();
};

#endif // MAINWINDOW_H
