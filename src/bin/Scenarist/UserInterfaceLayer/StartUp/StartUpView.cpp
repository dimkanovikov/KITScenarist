#include "StartUpView.h"
#include "ui_StartUpView.h"

#include "RecentFilesDelegate.h"

#include <QStandardItemModel>

using UserInterface::StartUpView;
using UserInterface::RecentFilesDelegate;


StartUpView::StartUpView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StartUpView)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

StartUpView::~StartUpView()
{
	delete ui;
}

void StartUpView::setRecentProjects(QAbstractItemModel* _recentProjectsModel)
{
	//
	// Если в списке была установлена модель, удалим её
	//
	if (ui->recentFiles->model() != 0) {
		QAbstractItemModel* oldModel = ui->recentFiles->model();
		ui->recentFiles->setModel(0);
		delete oldModel;
		oldModel = 0;
	}

	//
	// Установим новую модель
	//
	_recentProjectsModel->setParent(ui->recentFiles);
	ui->recentFiles->setModel(_recentProjectsModel);
}

void StartUpView::setUpdateInfo(const QString& _updateInfo)
{
	ui->updateInfo->setText(_updateInfo);
	ui->updateInfo->show();
}

void StartUpView::setUserLogged(bool isLogged, const QString& _userName)
{
	ui->loginIcon->setVisible(!isLogged);
	ui->login->setVisible(!isLogged);
	ui->logoutIcon->setVisible(isLogged);
	ui->logout->setVisible(isLogged);
	ui->logout->setText(QString("<a href=\"#\" style=\"color:#2b78da;\">%1</a> %2").arg(tr("Logout")).arg(_userName));
	ui->remoteProjects->setVisible(isLogged);

	if (!isLogged && ui->remoteProjects->isChecked()) {
		ui->localProjects->setChecked(true);
	}
}

void StartUpView::setRemoteProjects(QAbstractItemModel* _remoteProjectsModel)
{
	//
	// Если в списке была установлена модель, удалим её
	//
	if (ui->remoteFiles->model() != 0) {
		QAbstractItemModel* oldModel = ui->remoteFiles->model();
		ui->remoteFiles->setModel(0);
		delete oldModel;
		oldModel = 0;
	}

	//
	// Установим новую модель
	//
	_remoteProjectsModel->setParent(ui->remoteFiles);
	ui->remoteFiles->setModel(_remoteProjectsModel);
}

bool StartUpView::eventFilter(QObject* _watched, QEvent* _event)
{
	bool result = false;

	//
	// Когда мышка входит или покидает список недавних файлов, нужно перерисовать его,
	// чтобы не осталось невыделенных/выделенных модулей
	//
	if ((_watched == ui->recentFiles || _watched == ui->remoteFiles)
		&& (_event->type () == QEvent::Enter || _event->type () == QEvent::Leave))
	{
		if (_watched == ui->recentFiles) {
			ui->recentFiles->repaint();
		} else {
			ui->remoteFiles->repaint();
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
	if (ui->localProjects->isChecked()) {
		ui->filesSouces->setCurrentWidget(ui->recentFilesPage);
	} else {
		ui->filesSouces->setCurrentWidget(ui->remoteFilesPage);
	}
}

void StartUpView::initView()
{
	ui->version->setText(QApplication::applicationVersion());

	ui->updateInfo->hide();

	ui->logoutIcon->hide();
	ui->logout->hide();

	ui->remoteProjects->hide();

	ui->filesSouces->setCurrentWidget(ui->recentFilesPage);

	ui->recentFiles->setItemDelegate(new RecentFilesDelegate(ui->recentFiles));
	ui->recentFiles->setMouseTracking(true);
	ui->recentFiles->installEventFilter(this);
	ui->remoteFiles->setItemDelegate(new RecentFilesDelegate(ui->remoteFiles));
	ui->remoteFiles->setMouseTracking(true);
	ui->remoteFiles->installEventFilter(this);
}

void StartUpView::initConnections()
{
	connect(ui->login, SIGNAL(linkActivated(QString)), this, SIGNAL(loginClicked()));
	connect(ui->logout, SIGNAL(linkActivated(QString)), this, SIGNAL(logoutClicked()));
	connect(ui->createProject, SIGNAL(linkActivated(QString)), this, SIGNAL(createProjectClicked()));
	connect(ui->openProject, SIGNAL(linkActivated(QString)), this, SIGNAL(openProjectClicked()));
	connect(ui->help, SIGNAL(linkActivated(QString)), this, SIGNAL(helpClicked()));

	connect(ui->localProjects, SIGNAL(toggled(bool)), this, SLOT(aboutFilesSourceChanged()));
	connect(ui->recentFiles, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openRecentProjectClicked(QModelIndex)));
	connect(ui->remoteFiles, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openRemoteProjectClicked(QModelIndex)));
	connect(ui->refreshProjects, SIGNAL(clicked()), this, SIGNAL(refreshProjects()));
}

void StartUpView::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->localProjects->setProperty("inStartUpView", true);
	ui->remoteProjects->setProperty("inStartUpView", true);
}
