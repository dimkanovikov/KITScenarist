#include "ProjectFileWidget.h"
#include "ui_ProjectFileWidget.h"

#include "ProjectUserWidget.h"

#include <3rd_party/Widgets/ElidedLabel/ElidedLabel.h>
#include <3rd_party/Widgets/WAF/Animation.h>

#include <QLabel>
#include <QVBoxLayout>

using UserInterface::ProjectFileWidget;
using UserInterface::ProjectUserWidget;


ProjectFileWidget::ProjectFileWidget(QWidget *parent) :
	QFrame(parent),
	m_ui(new Ui::ProjectFileWidget)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStylesheet();
}

void ProjectFileWidget::setProjectName(const QString& _projectName)
{
	m_ui->projectName->setText(_projectName);
}

void ProjectFileWidget::setFilePath(const QString& _filePath)
{
	m_ui->filePath->setText(_filePath);
}

void ProjectFileWidget::addCollaborator(const QString& _email, const QString& _name, const QString& _role)
{
	ProjectUserWidget* user = new ProjectUserWidget(this);
	user->setUserInfo(_email, _name, _role);
	connect(user, &ProjectUserWidget::removeUserRequested, this, &ProjectFileWidget::removeUserRequested);
	m_ui->mainLayout->addWidget(user);

	m_users.append(user);
}

void ProjectFileWidget::setMouseHover(bool _hover)
{
	//
	// Выделяем если курсор над виджетом
	//
	QString styleSheet;
	if (_hover) {
		styleSheet = "background-color: palette(alternate-base);";
	} else {
		styleSheet = "background-color: palette(base);";
	}
	setStyleSheet(styleSheet);

	//
	// Показываем, или скрываем кнопку параметров
	//
	if (_hover || m_ui->options->isChecked()) {
		m_ui->options->show();
	} else {
		m_ui->options->hide();
	}
}

void ProjectFileWidget::enterEvent(QEvent* _event)
{
	setMouseHover(true);
	QWidget::enterEvent(_event);
}

void ProjectFileWidget::leaveEvent(QEvent* _event)
{
	setMouseHover(false);
	QWidget::leaveEvent(_event);
}

void ProjectFileWidget::mousePressEvent(QMouseEvent* _event)
{
	emit clicked();
	QWidget::mousePressEvent(_event);
}

void ProjectFileWidget::initView()
{
	setMouseTracking(true);
	setMouseHover(false);

	m_ui->filePath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_ui->filePath->setElideMode(Qt::ElideLeft);

	m_ui->change->setIcons(m_ui->change->icon());
	m_ui->remove->setIcons(m_ui->remove->icon());
	m_ui->hide->setIcons(m_ui->hide->icon());
	m_ui->share->setIcons(m_ui->share->icon());
	m_ui->options->setIcons(m_ui->options->icon());

	m_ui->optionsPanel->hide();
}

void ProjectFileWidget::initConnections()
{
	connect(m_ui->options, &FlatButton::toggled, [=] (bool _toggled) {
		WAF::Animation::slide(m_ui->optionsPanel, WAF::FromRightToLeft, true, _toggled);
	});
}

void ProjectFileWidget::initStylesheet()
{
	m_ui->filePath->setStyleSheet("color: palette(mid);");
}
