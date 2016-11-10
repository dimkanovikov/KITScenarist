#include "ProjectUserWidget.h"
#include "ui_ProjectUserWidget.h"

using UserInterface::ProjectUserWidget;


ProjectUserWidget::ProjectUserWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ProjectUserWidget)
{
	m_ui->setupUi(this);
}

ProjectUserWidget::~ProjectUserWidget()
{
	delete m_ui;
}

void ProjectUserWidget::setUserInfo(const QString& _email, const QString& _name, const QString& _role)
{
	m_ui->userName->setText(QString("%1 [%2]").arg(_name, _role));
	m_ui->userName->setToolTip(_email);
}

void ProjectUserWidget::initConnections()
{
	connect(m_ui->closeAccess, &QToolButton::clicked, [=] {
		emit removeUser(m_ui->userName->toolTip());
	});
}
