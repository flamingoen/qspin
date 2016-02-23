#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private slots:
    void loadFile();
    void runSimulate();
    void runVerify();
    void saveFile();
    std::string getRunOptions();
    std::string getCompileOptions();
};

#endif // MAINWINDOW_H
