#include "MenuView.h"
#include "ui_MenuView.h"

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

void MenuView::initView()
{
	setMinimumWidth(qMin(320, parentWidget()->width() - 56));
}

void MenuView::initConnections()
{

}

void MenuView::initStyleSheet()
{
	m_ui->content->setProperty("side-nav", true);
}
