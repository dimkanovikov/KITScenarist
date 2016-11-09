#include "StartUpView.h"
#include "ui_StartUpView.h"

#include "RecentFilesDelegate.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QStandardItemModel>

#include "ChangePasswordDialog.h"
#include "RenewSubscriptionDialog.h"

using UserInterface::StartUpView;
using UserInterface::RecentFilesDelegate;
using UserInterface::ChangePasswordDialog;
using UserInterface::RenewSubscriptionDialog;


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
    //ui->logoutIcon->setVisible(isLogged);
    //ui->logout->setVisible(isLogged);
    //ui->logout->setText(QString("%1 <a href=\"#\" style=\"color:#2b78da;\">%2</a>").arg(_userName).arg(tr("Logout")));
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

bool StartUpView::event(QEvent* _event)
{
	if (_event->type() == QEvent::PaletteChange) {
		initStyleSheet();
		initIconsColor();
	}

	return QWidget::event(_event);
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

void StartUpView::changePassword()
{
    ChangePasswordDialog dialog(this);
    if(dialog.exec() == QLightBoxDialog::Accepted) {
        //FIXME: дописать, когда появится реализация
    }
}

void StartUpView::renewSubscription()
{
    RenewSubscriptionDialog dialog(this);
    if(dialog.exec() == QLightBoxDialog::Accepted) {
        //FIXME: дописать, когда появится реализация
    }
}

void StartUpView::initView()
{
	QFont versionFont = ui->version->font();
#ifdef Q_OS_LINUX
	versionFont.setPointSize(10);
#elif defined Q_OS_MAC
	versionFont.setPointSize(12);
#endif
	ui->version->setFont(versionFont);
	ui->version_2->setFont(versionFont);

	ui->version->setText(QApplication::applicationVersion());

	ui->updateInfo->hide();

    //ui->logoutIcon->hide();
    //ui->logout->hide();

	ui->remoteProjects->hide();

	ui->filesSouces->setCurrentWidget(ui->recentFilesPage);

	ui->recentFiles->setItemDelegate(new RecentFilesDelegate(ui->recentFiles));
	ui->recentFiles->setMouseTracking(true);
	ui->recentFiles->installEventFilter(this);
	ui->remoteFiles->setItemDelegate(new RecentFilesDelegate(ui->remoteFiles));
	ui->remoteFiles->setMouseTracking(true);
	ui->remoteFiles->installEventFilter(this);

	initIconsColor();
}

void StartUpView::initConnections()
{
	connect(ui->login, SIGNAL(linkActivated(QString)), this, SIGNAL(loginClicked()));
    connect(ui->logout, &QPushButton::clicked, this, &StartUpView::logoutClicked);;
	connect(ui->createProject, SIGNAL(clicked(bool)), this, SIGNAL(createProjectClicked()));
	connect(ui->openProject, SIGNAL(clicked(bool)), this, SIGNAL(openProjectClicked()));
	connect(ui->help, SIGNAL(clicked(bool)), this, SIGNAL(helpClicked()));

	connect(ui->localProjects, SIGNAL(toggled(bool)), this, SLOT(aboutFilesSourceChanged()));
	connect(ui->recentFiles, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openRecentProjectClicked(QModelIndex)));
	connect(ui->remoteFiles, SIGNAL(clicked(QModelIndex)), this, SIGNAL(openRemoteProjectClicked(QModelIndex)));
	connect(ui->refreshProjects, SIGNAL(clicked()), this, SIGNAL(refreshProjects()));
    connect(ui->changePassword, &QPushButton::clicked, this, &StartUpView::changePassword);
    connect(ui->renewSubscription, &QPushButton::clicked, this, &StartUpView::renewSubscription);

    connect(ui->toProjects, &QPushButton::clicked, [this] {
        ui->stackedWidget->setCurrentWidget(ui->projectsPage);
    });
    connect(ui->toCabinet, &QPushButton::clicked, [this] {
        ui->stackedWidget->setCurrentWidget(ui->cabinetPage);
    });
}

void StartUpView::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
    //ui->projectsFrame->setProperty("mainContainer", true);
    //ui->projectsFrame->setProperty("baseForeground", true);
    ui->stackedWidget->setProperty("mainContainer", true);
    ui->stackedWidget->setProperty("baseForeground", true);

	ui->createProject->setProperty("leftAlignedText", true);
	ui->openProject->setProperty("leftAlignedText", true);
	ui->help->setProperty("leftAlignedText", true);

    ui->cabinetHeader->setProperty("inStartUpView", true);
	ui->localProjects->setProperty("inStartUpView", true);
	ui->remoteProjects->setProperty("inStartUpView", true);

	ui->recentFiles->viewport()->setStyleSheet("background-color: palette(base);");
	ui->remoteFiles->viewport()->setStyleSheet("background-color: palette(base);");
}

void StartUpView::initIconsColor()
{
	const QSize iconSize = ui->createProject->iconSize();

	QIcon account(*ui->loginIcon->pixmap());
	ImageHelper::setIconColor(account, iconSize, palette().text().color());
	ui->loginIcon->setPixmap(account.pixmap(iconSize));
    //ui->logoutIcon->setPixmap(account.pixmap(iconSize));

	QIcon createProject = ui->createProject->icon();
	ImageHelper::setIconColor(createProject, iconSize, palette().text().color());
	ui->createProject->setIcon(createProject);

	QIcon openProject = ui->openProject->icon();
	ImageHelper::setIconColor(openProject, iconSize, palette().text().color());
	ui->openProject->setIcon(openProject);

	QIcon help = ui->help->icon();
	ImageHelper::setIconColor(help, iconSize, palette().text().color());
	ui->help->setIcon(help);

	QIcon refresh = ui->refreshProjects->icon();
	ImageHelper::setIconColor(refresh, iconSize, palette().text().color());
	ui->refreshProjects->setIcon(refresh);
}
