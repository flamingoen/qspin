#include "settingswindow.h"

settingsWindow::settingsWindow() : QDialog(0,0), ui(new Ui::options){

    ui->setupUi(this);
    this->setWindowTitle("Configure");

    comboBoxCCompiler = this->findChild<QComboBox *>    ("ComboBoxCompiler");
    comboBoxSpinCmd =   this->findChild<QComboBox *>    ("comboBoxSpin");
    browseC =           this->findChild<QPushButton *>  ("pushButtonCompiler");
    browseSpin =        this->findChild<QPushButton *>  ("pushButtonSpin");

    connect(browseC, SIGNAL(clicked()), this, SLOT(browseForCompiler()));
    connect(browseSpin, SIGNAL(clicked()), this, SLOT(browseForSpin()));

    QSettings settings;
    QString compiler = settings.value("settings/compiler",CCOMPILER).toString();
    QString spin = settings.value("settings/spin",SPIN).toString();
    comboBoxCCompiler->addItem(compiler);
    comboBoxSpinCmd->addItem(spin);
}

void settingsWindow::saveSettings() {
    QSettings settings;

    settings.setValue("settings/compiler",comboBoxCCompiler->currentText());
    settings.setValue("settings/spin",comboBoxSpinCmd->currentText());

}

QString settingsWindow::getCompiler() {
    return comboBoxCCompiler->currentText();
}

QString settingsWindow::getSpin() {
    return comboBoxSpinCmd->currentText();
}

void settingsWindow::browseForCompiler() {
    QString path = browse();
    if (path!=NULL) {
        comboBoxCCompiler->addItem(path);
        comboBoxCCompiler->setCurrentIndex(comboBoxCCompiler->currentIndex()+1);
    }
}

void settingsWindow::browseForSpin() {
    QString path = browse();
    if (path!=NULL) {
        comboBoxSpinCmd->addItem(path);
        comboBoxSpinCmd->setCurrentIndex(comboBoxSpinCmd->currentIndex()+1);
    }
}

QString settingsWindow::browse() {
    return QFileDialog::getOpenFileName(this, tr("Browse"),"",tr("All Files (*);;.exe Files (*.exe)"));
}
