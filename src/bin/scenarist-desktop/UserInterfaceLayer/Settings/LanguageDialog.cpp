#include "LanguageDialog.h"
#include "ui_LanguageDialog.h"

using UserInterface::LanguageDialog;


LanguageDialog::LanguageDialog(QWidget *parent, int _language) :
    QLightBoxDialog(parent),
    m_ui(new Ui::LanguageDialog)
{
    m_ui->setupUi(this);

    if (_language == -1) {
        m_ui->system->setChecked(true);
    } else if (_language == 0) {
        m_ui->russian->setChecked(true);
    } else if (_language == 1) {
        m_ui->spanish->setChecked(true);
    } else if (_language == 2) {
        m_ui->english->setChecked(true);
    } else if (_language == 3) {
        m_ui->french->setChecked(true);
    } else if (_language == 4) {
        m_ui->kazakh->setChecked(true);
    } else if (_language == 5) {
        m_ui->ukrainian->setChecked(true);
    } else if (_language == 6) {
        m_ui->german->setChecked(true);
    } else if (_language == 7) {
        m_ui->portugues->setChecked(true);
    } else if (_language == 8) {
        m_ui->farsi->setChecked(true);
    } else if (_language == 9) {
        m_ui->chinese->setChecked(true);
    }
}

LanguageDialog::~LanguageDialog()
{
    delete m_ui;
}

int LanguageDialog::language() const
{
    int result = -1;
    if (m_ui->system->isChecked()) {
        result = -1;
    } else if (m_ui->russian->isChecked()) {
        result = 0;
    } else if (m_ui->spanish->isChecked()) {
        result = 1;
    } else if (m_ui->english->isChecked()) {
        result = 2;
    } else if (m_ui->french->isChecked()) {
        result = 3;
    } else if (m_ui->kazakh->isChecked()) {
        result = 4;
    } else if (m_ui->ukrainian->isChecked()) {
        result = 5;
    } else if (m_ui->german->isChecked()) {
        result = 6;
    } else if (m_ui->portugues->isChecked()) {
        result = 7;
    } else if (m_ui->farsi->isChecked()) {
        result = 8;
    } else if (m_ui->chinese->isChecked()) {
        result = 9;
    }

    return result;
}

void LanguageDialog::initView()
{
}

void LanguageDialog::initConnections()
{
    connect(m_ui->buttons, SIGNAL(accepted()), this, SLOT(accept()));
}
