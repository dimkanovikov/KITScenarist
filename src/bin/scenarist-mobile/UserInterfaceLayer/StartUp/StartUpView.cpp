#include "StartUpView.h"
#include "ui_StartUpView.h"

#include "RecentFilesDelegate.h"

#include <3rd_party/Helpers/ImageHelper.h>

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

void StartUpView::setRecentProjects(QAbstractItemModel* _recentProjectsModel)
{
	//
	// Если в списке была установлена модель, удалим её
	//
	if (m_ui->recentFiles->model() != 0) {
		QAbstractItemModel* oldModel = m_ui->recentFiles->model();
		m_ui->recentFiles->setModel(0);
		delete oldModel;
		oldModel = 0;
	}

	//
	// Установим новую модель
	//
	_recentProjectsModel->setParent(m_ui->recentFiles);
	m_ui->recentFiles->setModel(_recentProjectsModel);
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
	if ((_watched == m_ui->recentFiles || _watched == m_ui->remoteFiles)
		&& (_event->type () == QEvent::Enter || _event->type () == QEvent::Leave))
	{
		if (_watched == m_ui->recentFiles) {
			m_ui->recentFiles->repaint();
		} else {
			m_ui->remoteFiles->repaint();
		}
		result = true;
	}
	//
	// Для всех остальных событий используем реализацю базовового класса
	//
	else
	{
		result = QWidget::eventFilter(_watched, _event);
	}

	return result;
}

void StartUpView::aboutFilesSourceChanged()
{
	if (m_ui->localProjects->isChecked()) {
		m_ui->filesSouces->setCurrentWidget(m_ui->recentFilesPage);
	} else {
		m_ui->filesSouces->setCurrentWidget(m_ui->remoteFilesPage);
	}
}

void StartUpView::initView()
{
	m_ui->remoteProjects->hide();

	m_ui->filesSouces->setCurrentWidget(m_ui->recentFilesPage);

	m_ui->recentFiles->setItemDelegate(new RecentFilesDelegate(m_ui->recentFiles));
	m_ui->recentFiles->setMouseTracking(true);
	m_ui->recentFiles->installEventFilter(this);
	m_ui->remoteFiles->setItemDelegate(new RecentFilesDelegate(m_ui->remoteFiles));
	m_ui->remoteFiles->setMouseTracking(true);
	m_ui->remoteFiles->installEventFilter(this);
}

void StartUpView::initConnections()
{
	connect(m_ui->createProject, SIGNAL(clicked(bool)), this, SIGNAL(createProjectClicked()));

	connect(m_ui->localProjects, SIGNAL(toggled(bool)), this, SLOT(aboutFilesSourceChanged()));
	connect(m_ui->recentFiles, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openRecentProjectClicked(QModelIndex)));
	connect(m_ui->remoteFiles, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openRemoteProjectClicked(QModelIndex)));
}

void StartUpView::initStyleSheet()
{
}
