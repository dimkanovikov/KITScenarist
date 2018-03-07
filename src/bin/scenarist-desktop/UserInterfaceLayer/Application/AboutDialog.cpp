#include "AboutDialog.h"
#include "ui_AboutDialog.h"

using UserInterface::AboutDialog;


AboutDialog::AboutDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::AboutDialog)
{
    m_ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete m_ui;
}

void AboutDialog::initView()
{

}

void AboutDialog::initConnections()
{
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &AboutDialog::accept);
}
