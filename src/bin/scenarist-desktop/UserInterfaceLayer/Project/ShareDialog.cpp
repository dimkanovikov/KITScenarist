#include "ShareDialog.h"
#include "ui_ShareDialog.h"

#include <3rd_party/Helpers/Validators.h>

using UserInterface::ShareDialog;


ShareDialog::ShareDialog(QWidget *parent) :
    QLightBoxDialog(parent),
    m_ui(new Ui::ShareDialog)
{
    m_ui->setupUi(this);
}

ShareDialog::~ShareDialog()
{
    delete m_ui;
}

QString ShareDialog::email() const
{
    return m_ui->email->text().simplified();
}

int ShareDialog::role() const
{
    return m_ui->role->currentIndex();
}

QWidget* ShareDialog::focusedOnExec() const
{
    return m_ui->email;
}

void ShareDialog::initView()
{
    m_ui->buttons->addButton(tr("Share"), QDialogButtonBox::AcceptRole);
}

void ShareDialog::initConnections()
{
    connect(m_ui->buttons, &QDialogButtonBox::accepted, [=] {
        if (Validator::isEmailValid(email())) {
            accept();
        } else {
            //
            // TODO: Показать ошибку о том, что плохой адрес электронной почты
            //
        }
    });
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &ShareDialog::reject);
}
