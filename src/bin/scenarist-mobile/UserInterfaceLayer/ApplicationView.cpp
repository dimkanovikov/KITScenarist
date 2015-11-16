#include "ApplicationView.h"
#include "ui_ApplicationView.h"

#include <QCloseEvent>

using UserInterface::ApplicationView;


ApplicationView::ApplicationView(QWidget *parent) :
    QOpenGLWidget(parent),
	m_ui(new Ui::ApplicationView)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
    initStyleSheet();
	setAutoFillBackground(false);
}

ApplicationView::~ApplicationView()
{
	delete m_ui;
}

void ApplicationView::addView(QWidget* _toolbar, QWidget* _view)
{
	m_ui->toolbarsContainer->addWidget(_toolbar);
	m_ui->viewsContainer->addWidget(_view);
}

void ApplicationView::setCurrentView(int _index)
{
	m_ui->toolbarsContainer->setCurrentIndex(_index);
	m_ui->viewsContainer->setCurrentIndex(_index);
}

void ApplicationView::closeEvent(QCloseEvent* _event)
{
	//
	// Вместо реального закрытия формы испускаем сигнал сигнализирующий об этом намерении
	//

	_event->ignore();
    emit wantToClose();
}

void ApplicationView::initView()
{

}

void ApplicationView::initConnections()
{
	connect(m_ui->menu, &QToolButton::clicked, this, &ApplicationView::menuClicked);
}

void ApplicationView::initStyleSheet()
{
	m_ui->toolbar->setProperty("toolbar", true);
}
