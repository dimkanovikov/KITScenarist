#include "AddProjectDialog.h"
#include "ui_AddProjectDialog.h"

using UserInterface::AddProjectDialog;


AddProjectDialog::AddProjectDialog(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::AddProjectDialog)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

AddProjectDialog::~AddProjectDialog()
{
	delete m_ui;
}

QString AddProjectDialog::projectName() const
{
	return m_ui->projectName->text();
}

void AddProjectDialog::initView()
{

}

void AddProjectDialog::initConnections()
{
	connect(m_ui->create, &QPushButton::clicked, this, &AddProjectDialog::createClicked);
	connect(m_ui->cancel, &QPushButton::clicked, this, &AddProjectDialog::cancelClicked);
}

void AddProjectDialog::initStyleSheet()
{
	m_ui->create->setProperty("raised", true);
	m_ui->cancel->setProperty("raised", true);
}
