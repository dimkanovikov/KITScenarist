#include "LanguageDialog.h"
#include "ui_LanguageDialog.h"

using UserInterface::LanguageDialog;


LanguageDialog::LanguageDialog(QWidget *parent, int _language) :
    QLightBoxDialog(parent),
    ui(new Ui::LanguageDialog)
{
    ui->setupUi(this);

    if (_language == -1) {
        ui->system->setChecked(true);
    } else if (_language == 0) {
        ui->russian->setChecked(true);
    } else if (_language == 1) {
        ui->spanish->setChecked(true);
    } else if (_language == 2) {
        ui->english->setChecked(true);
    } else if (_language == 3) {
        ui->french->setChecked(true);
    } else if (_language == 4) {
        ui->kazakh->setChecked(true);
    } else if (_language == 5) {
        ui->ukrainian->setChecked(true);
    } else if (_language == 6) {
        ui->german->setChecked(true);
    } else if (_language == 7) {
        ui->portugues->setChecked(true);
    }
}

LanguageDialog::~LanguageDialog()
{
    delete ui;
}

int LanguageDialog::language() const
{
    int result = -1;
    if (ui->system->isChecked()) {
        result = -1;
    } else if (ui->russian->isChecked()) {
        result = 0;
    } else if (ui->spanish->isChecked()) {
        result = 1;
    } else if (ui->english->isChecked()) {
        result = 2;
    } else if (ui->french->isChecked()) {
        result = 3;
    } else if (ui->kazakh->isChecked()) {
        result = 4;
    } else if (ui->ukrainian->isChecked()) {
        result = 5;
    } else if (ui->german->isChecked()) {
        result = 6;
    } else if (ui->portugues->isChecked()) {
        result = 7;
    }

    return result;
}

void LanguageDialog::initView()
{
}

void LanguageDialog::initConnections()
{
    connect(ui->buttons, SIGNAL(accepted()), this, SLOT(accept()));
}
