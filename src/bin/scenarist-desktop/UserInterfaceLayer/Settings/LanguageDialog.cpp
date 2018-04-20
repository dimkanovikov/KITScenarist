#include "LanguageDialog.h"
#include "ui_LanguageDialog.h"

using UserInterface::LanguageDialog;


LanguageDialog::LanguageDialog(QWidget *parent, int _language) :
    QLightBoxDialog(parent),
    m_ui(new Ui::LanguageDialog)
{
    m_ui->setupUi(this);

    QRadioButton* languageButton = nullptr;
    switch (_language) {
        case -1: languageButton = m_ui->system; break;
        case 0: languageButton = m_ui->russian; break;
        case 1: languageButton = m_ui->spanish; break;
        case 2: languageButton = m_ui->english; break;
        case 3: languageButton = m_ui->french; break;
        case 4: languageButton = m_ui->kazakh; break;
        case 5: languageButton = m_ui->ukrainian; break;
        case 6: languageButton = m_ui->german; break;
        case 7: languageButton = m_ui->portugues; break;
        case 8: languageButton = m_ui->farsi; break;
        case 9: languageButton = m_ui->chinese; break;
        case 10: languageButton = m_ui->hebrew; break;
        case 11: languageButton = m_ui->polish; break;
        case 12: languageButton = m_ui->turkish; break;
        default: break;
    }

    if (languageButton != nullptr) {
        languageButton->setChecked(true);
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
    } else if (m_ui->hebrew->isChecked()) {
        result = 10;
    } else if (m_ui->polish->isChecked()) {
        result = 11;
    } else if (m_ui->turkish->isChecked()) {
        result = 12;
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
