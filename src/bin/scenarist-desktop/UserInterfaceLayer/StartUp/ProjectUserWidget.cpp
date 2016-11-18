#include "ProjectUserWidget.h"
#include "ui_ProjectUserWidget.h"

using UserInterface::ProjectUserWidget;


ProjectUserWidget::ProjectUserWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ProjectUserWidget)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
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

void ProjectUserWidget::setDeletable(bool _isDeletable)
{
	m_ui->closeAccess->setEnabled(_isDeletable);
}

void ProjectUserWidget::initView()
{
	m_ui->closeAccess->setIcons(m_ui->closeAccess->icon());
}

void ProjectUserWidget::initConnections()
{
	connect(m_ui->closeAccess, &QToolButton::clicked, [=] {
		emit removeUserRequested(m_ui->userName->toolTip());
	});
}
