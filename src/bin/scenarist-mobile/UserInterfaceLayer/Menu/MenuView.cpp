#include "MenuView.h"
#include "ui_MenuView.h"

#include <3rd_party/Helpers/StyleSheetHelper.h>

using UserInterface::MenuView;


MenuView::MenuView(QWidget* _parent) :
	QWidget(_parent),
	m_ui(new Ui::MenuView)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

MenuView::~MenuView()
{
	delete m_ui;
}

void MenuView::setUserLogged(const QString& _userName)
{
	m_ui->cabin->setText("     " + _userName);
}

void MenuView::showProjectSubmenu(const QString& _projectName)
{
	const QFontMetrics metrics = m_ui->projectName->fontMetrics();
	const int margin = 80;
	m_ui->projectName->setText(metrics.elidedText(_projectName, Qt::ElideRight, width() - margin));

	m_ui->projectName->show();
	m_ui->projectSave->show();
	m_ui->projectText->show();

	m_ui->projectText->setChecked(true);
}

void MenuView::hideProjectSubmenu()
{
	m_ui->projectName->hide();
	m_ui->projectSave->hide();
	m_ui->projectText->hide();
}

void MenuView::initView()
{
	setFixedWidth(
		StyleSheetHelper::dpToPx(
			qMin(400,
				 qMin(parentWidget()->width(), parentWidget()->height()) - 56
				 )
			)
		);

	QButtonGroup* menuButtonsGroup = new QButtonGroup(this);
	menuButtonsGroup->setExclusive(true);
	menuButtonsGroup->addButton(m_ui->projects);
	menuButtonsGroup->addButton(m_ui->projectText);

	hideProjectSubmenu();
}

void MenuView::initConnections()
{
	connect(m_ui->close, &QToolButton::clicked, this, &MenuView::backClicked);
	connect(m_ui->cabin, &QToolButton::clicked, this, &MenuView::cabinClicked);
	connect(m_ui->projects, &QToolButton::clicked, this, &MenuView::projectsClicked);
	connect(m_ui->projectSave, &QToolButton::clicked, this, &MenuView::projectSaveClicked);
	connect(m_ui->projectText, &QToolButton::clicked, this, &MenuView::projectTextClicked);
	connect(m_ui->settings, &QToolButton::clicked, this, &MenuView::settingsClicked);
}

void MenuView::initStyleSheet()
{
	m_ui->content->setProperty("side-nav", true);
	m_ui->cabin->setProperty("side-nav-name", true);
	m_ui->projects->setProperty("side-nav-item", true);
	m_ui->projectName->setProperty("side-nav-subheader", true);
	m_ui->projectSave->setProperty("side-nav-item", true);
	m_ui->projectText->setProperty("side-nav-item", true);
	m_ui->settings->setProperty("side-nav-item", true);
}
