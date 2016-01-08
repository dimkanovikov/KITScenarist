#include "CabinView.h"
#include "ui_CabinView.h"

using UserInterface::CabinView;


CabinView::CabinView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::CabinView)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

CabinView::~CabinView()
{
	delete m_ui;
}

QWidget* CabinView::toolbar()
{
	return m_ui->toolbar;
}

void CabinView::initView()
{

}

void CabinView::initConnections()
{
	connect(m_ui->logout, &QToolButton::clicked, this, &CabinView::logoutRequested);
}

void CabinView::initStyleSheet()
{
	m_ui->toolbar->setProperty("toolbar", true);
	m_ui->title->setProperty("toolbar", true);
}
