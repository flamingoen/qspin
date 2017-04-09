#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QComboBox>
#include <QDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include "ui_options.h"

#ifdef _WIN32
    #define CCOMPILER "C:\MinGW\bin\gcc.exe"
    #define SPIN "spin\spin.exe"
#else
    #define CCOMPILER "cc"
    #define SPIN "spin"
#endif

class settingsWindow : public QDialog {
    Q_OBJECT

private:
    Ui::options *ui;
    QComboBox *comboBoxCCompiler;
    QComboBox *comboBoxSpinCmd;
    QPushButton *browseC;
    QPushButton *browseSpin;

    QString browse();

public:
    settingsWindow();
    void saveSettings();
    QString getCompiler();
    QString getSpin();

signals:

public slots:

private slots:
    void browseForCompiler();
    void browseForSpin();

};

#endif // SETTINGSWINDOW_H
