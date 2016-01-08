#include "StartUpView.h"
#include "ui_StartUpView.h"

#include "RecentFilesDelegate.h"

#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Helpers/ScrollerHelper.h>

#include <QStandardItemModel>

using UserInterface::StartUpView;
using UserInterface::RecentFilesDelegate;


StartUpView::StartUpView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::StartUpView)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

StartUpView::~StartUpView()
{
	delete m_ui;
}

QWidget* StartUpView::toolbar() const
{
	return m_ui->toolbar;
}

void StartUpView::setRemoteProjects(QAbstractItemModel* _remoteProjectsModel)
{
	//
	// Если в списке была установлена модель, удалим её
	//
	if (m_ui->remoteFiles->model() != 0) {
		QAbstractItemModel* oldModel = m_ui->remoteFiles->model();
		m_ui->remoteFiles->setModel(0);
		delete oldModel;
		oldModel = 0;
	}

	//
	// Установим новую модель
	//
	_remoteProjectsModel->setParent(m_ui->remoteFiles);
	m_ui->remoteFiles->setModel(_remoteProjectsModel);
}

bool StartUpView::eventFilter(QObject* _watched, QEvent* _event)
{
	bool result = false;

	//
	// Когда мышка входит или покидает список недавних файлов, нужно перерисовать его,
	// чтобы не осталось невыделенных/выделенных модулей
	//
	if (_watched == m_ui->remoteFiles
		&& (_event->type () == QEvent::Enter || _event->type () == QEvent::Leave)) {
		m_ui->remoteFiles->repaint();
		result = true;
	}
	//
	// Для всех остальных событий используем реализацю базовового класса
	//
	else {
		result = QWidget::eventFilter(_watched, _event);
	}

	return result;
}

void StartUpView::initView()
{
	m_ui->createProject->hide();

	ScrollerHelper::addScroller(m_ui->remoteFiles);
	m_ui->remoteFiles->setItemDelegate(new RecentFilesDelegate(m_ui->remoteFiles));
	m_ui->remoteFiles->setMouseTracking(true);
	m_ui->remoteFiles->installEventFilter(this);
}

void StartUpView::initConnections()
{
	connect(m_ui->createProject, SIGNAL(clicked(bool)), this, SIGNAL(createProjectClicked()));
	connect(m_ui->remoteFiles, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openRemoteProjectClicked(QModelIndex)));
}

void StartUpView::initStyleSheet()
{
	m_ui->toolbar->setProperty("toolbar", true);
	m_ui->label->setProperty("toolbar", true);
}
