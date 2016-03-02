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

private slots:
    void loadFile();
    void runRandomSimulation();
    void runInteractiveSimulation();
    void runSubmitIteracticeSimulation();
    void runVerify();
    void saveFile();
    QStringList getRunOptions();
    QStringList getCompileOptions();
};

#endif // MAINWINDOW_H
