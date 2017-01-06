#include "StartUpView.h"
#include "ui_StartUpView.h"

#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>

#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

#include "ChangePasswordDialog.h"
#include "RenewSubscriptionDialog.h"

using UserInterface::StartUpView;
using UserInterface::ChangePasswordDialog;
using UserInterface::RenewSubscriptionDialog;
using UserInterface::ProjectsList;

using WAF::Animation;
using WAF::AnimationDirection;

namespace {
    const bool PROJECTS_IS_REMOTE = true;
    const bool PROJECTS_IS_LOCAL = false;
}


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

bool StartUpView::isOnLocalProjectsTab() const
{
    return ui->localProjects->isChecked();
}

void StartUpView::setRecentProjects(QAbstractItemModel* _recentProjectsModel)
{
    //
    // Если в списке была установлена модель, удалим её
    //
    if (ui->recentFiles->model() != 0) {
        QAbstractItemModel* oldModel = ui->recentFiles->model();
        ui->recentFiles->setModel(0, PROJECTS_IS_LOCAL);
        delete oldModel;
        oldModel = 0;
    }

    //
    // Установим новую модель
    //
    _recentProjectsModel->setParent(ui->recentFiles);
    ui->recentFiles->setModel(_recentProjectsModel, PROJECTS_IS_LOCAL);
}

void StartUpView::setUpdateInfo(const QString& _updateInfo)
{
    ui->updateInfo->setText(_updateInfo);
    ui->updateInfo->show();
}

void StartUpView::setUserLogged(bool isLogged, const QString& _userName, const QString& _userEmail)
{
    ui->remoteProjects->setVisible(isLogged);

    if (isLogged) {
        ui->userName->setAcceptedText(_userName);
        ui->userEmail->setText(QString("<a href=\"#\" style=\"color:#2b78da;\">%1</a>").arg(_userEmail));
        ui->login->hide();
        ui->userEmail->show();
    } else {
        Animation::slideOut(ui->cabinetFrame, AnimationDirection::FromTopToBottom, true);
        QTimer::singleShot(300, [this] {
            ui->cabinetFrame->hide();
            ui->userEmail->hide();
            ui->login->show();
        });
    }

    if (!isLogged && ui->remoteProjects->isChecked()) {
        ui->localProjects->setChecked(true);
    }
}

void StartUpView::setSubscriptionInfo(bool _isActive, const QString &_expDate)
{
    if (_isActive) {
        ui->subscriptionActivity->setText(tr("Account is active until:"));
        ui->subscriptionEndDate->setText(_expDate);
    } else {
        ui->subscriptionActivity->setText(tr("Account is inactive"));
        ui->subscriptionEndDate->clear();
    }
}

void StartUpView::setRemoteProjects(QAbstractItemModel* _remoteProjectsModel)
{
    //
    // Если в списке была установлена модель, удалим её
    //
    if (ui->remoteFiles->model() != 0) {
        QAbstractItemModel* oldModel = ui->remoteFiles->model();
        ui->remoteFiles->setModel(0, PROJECTS_IS_REMOTE);
        delete oldModel;
        oldModel = 0;
    }

    //
    // Установим новую модель
    //
    _remoteProjectsModel->setParent(ui->remoteFiles);
    ui->remoteFiles->setModel(_remoteProjectsModel, PROJECTS_IS_REMOTE);
}

bool StartUpView::event(QEvent* _event)
{
    if (_event->type() == QEvent::PaletteChange) {
        initStyleSheet();
        initIconsColor();
    }

    return QWidget::event(_event);
}

void StartUpView::aboutFilesSourceChanged()
{
    if (ui->localProjects->isChecked()) {
        ui->filesSouces->setCurrentWidget(ui->recentFilesPage);
    } else {
        ui->filesSouces->setCurrentWidget(ui->remoteFilesPage);
    }
}

void StartUpView::cabinetChangeVisibility()
{
    bool isVisible = ui->cabinetFrame->isVisible();
    if (!isVisible) {
        ui->cabinetFrame->show();
    }
    Animation::slide(ui->cabinetFrame, AnimationDirection::FromTopToBottom, true, !isVisible);
    if (isVisible) {
        QTimer::singleShot(300, ui->cabinetFrame, &QWidget::hide);
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

    ui->remoteProjects->hide();
    ui->cabinetFrame->hide();
    ui->userEmail->hide();

    ui->filesSouces->setCurrentWidget(ui->recentFilesPage);

    initIconsColor();
}

void StartUpView::initConnections()
{
    connect(ui->login, SIGNAL(linkActivated(QString)), this, SIGNAL(loginClicked()));
    connect(ui->userEmail, &QLabel::linkActivated, this, &StartUpView::cabinetChangeVisibility);
    connect(ui->logout, &QPushButton::clicked, this, &StartUpView::logoutClicked);
    connect(ui->createProject, SIGNAL(clicked(bool)), this, SIGNAL(createProjectClicked()));
    connect(ui->openProject, SIGNAL(clicked(bool)), this, SIGNAL(openProjectClicked()));
    connect(ui->help, SIGNAL(clicked(bool)), this, SIGNAL(helpClicked()));

    connect(ui->userName, &AcceptebleLineEdit::textAccepted, this, &StartUpView::userNameChanged);
    connect(ui->changePassword, &QPushButton::clicked, this, &StartUpView::passwordChangeClicked);
    connect(ui->getSubscriptionInfo, &QToolButton::clicked, this, &StartUpView::getSubscriptionInfoClicked);
    connect(ui->renewSubscription, &QPushButton::clicked, this, &StartUpView::renewSubscriptionClicked);

    connect(ui->localProjects, SIGNAL(toggled(bool)), this, SLOT(aboutFilesSourceChanged()));
    connect(ui->recentFiles, &ProjectsList::clicked, this, &StartUpView::openRecentProjectClicked);
    connect(ui->recentFiles, &ProjectsList::hideRequested, this, &StartUpView::hideRecentProjectRequested);
    connect(ui->remoteFiles, &ProjectsList::clicked, this, &StartUpView::openRemoteProjectClicked);
    connect(ui->remoteFiles, &ProjectsList::editRequested, this, &StartUpView::editRemoteProjectRequested);
    connect(ui->remoteFiles, &ProjectsList::removeRequested, this, &StartUpView::removeRemoteProjectRequested);
    connect(ui->remoteFiles, &ProjectsList::shareRequested, this, &StartUpView::shareRemoteProjectRequested);
    connect(ui->remoteFiles, &ProjectsList::unshareRequested, this, &StartUpView::unshareRemoteProjectRequested);
    connect(ui->refreshProjects, SIGNAL(clicked()), this, SIGNAL(refreshProjects()));
}

void StartUpView::initStyleSheet()
{
    ui->topEmptyLabel->setProperty("inTopPanel", true);
    ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
    ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

    ui->mainContainer->setProperty("mainContainer", true);
    ui->projectsFrame->setProperty("mainContainer", true);
    ui->projectsFrame->setProperty("baseForeground", true);

    ui->userName->setProperty("editableLabel", true);

    ui->getSubscriptionInfo->setProperty("leftAlignedText", true);
    ui->renewSubscription->setProperty("leftAlignedText", true);
    ui->changePassword->setProperty("leftAlignedText", true);
    ui->logout->setProperty("leftAlignedText", true);
    ui->createProject->setProperty("leftAlignedText", true);
    ui->openProject->setProperty("leftAlignedText", true);
    ui->help->setProperty("leftAlignedText", true);

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
    ui->getSubscriptionInfo->setIcon(refresh);
}
