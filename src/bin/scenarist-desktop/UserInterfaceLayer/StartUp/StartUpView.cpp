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

bool StartUpView::isOnLocalProjectsTab() const
{
    return m_ui->localProjects->isChecked();
}

void StartUpView::setRecentProjects(QAbstractItemModel* _recentProjectsModel)
{
    //
    // Если в списке была установлена модель, удалим её
    //
    if (m_ui->recentFiles->model() != 0) {
        QAbstractItemModel* oldModel = m_ui->recentFiles->model();
        m_ui->recentFiles->setModel(0, PROJECTS_IS_LOCAL);
        delete oldModel;
        oldModel = 0;
    }

    //
    // Установим новую модель
    //
    _recentProjectsModel->setParent(m_ui->recentFiles);
    m_ui->recentFiles->setModel(_recentProjectsModel, PROJECTS_IS_LOCAL);
}

void StartUpView::setUpdateInfo(const QString& _updateInfo)
{
    m_ui->updateInfo->setText(_updateInfo);
    m_ui->updateInfo->show();
}

void StartUpView::setUserLogged(bool isLogged, const QString& _userName, const QString& _userEmail)
{
    m_ui->remoteProjects->setVisible(isLogged);

    if (isLogged) {
        m_ui->userName->setAcceptedText(_userName);
        m_ui->userEmail->setText(QString("<a href=\"#\" style=\"color:#2b78da;\">%1</a>").arg(_userEmail));
        m_ui->login->hide();
        m_ui->userEmail->show();
    } else {
        Animation::slideOut(m_ui->cabinetFrame, AnimationDirection::FromTopToBottom, true);
        QTimer::singleShot(300, [this] {
            m_ui->cabinetFrame->hide();
            m_ui->userEmail->hide();
            m_ui->login->show();
        });
    }

    if (!isLogged && m_ui->remoteProjects->isChecked()) {
        m_ui->localProjects->setChecked(true);
    }
}

void StartUpView::setSubscriptionInfo(bool _isActive, const QString &_expDate, quint64 _usedSpace, quint64 _availableSpace)
{
    if (_isActive) {
        m_ui->subscriptionActivity->setText(tr("Account is active until:"));
        m_ui->subscriptionEndDate->setText(_expDate);
    } else {
        m_ui->subscriptionActivity->setText(tr("Account is inactive"));
        m_ui->subscriptionEndDate->clear();
    }


    m_ui->availableSpaceInfo->setText(
                tr("Used %1 Gb from %2 Gb")
                .arg(QString::number(qreal(_usedSpace / 1000000000.), 'f', 2))
                .arg(QString::number(qreal(_availableSpace / 1000000000.), 'f', 2)));
    m_ui->availableSpaceProgress->setMaximum(_availableSpace / 1000000);
    m_ui->availableSpaceProgress->setValue(_usedSpace / 1000000);
}

void StartUpView::setRemoteProjects(QAbstractItemModel* _remoteProjectsModel)
{
    //
    // Если в списке была установлена модель, удалим её
    //
    if (m_ui->remoteFiles->model() != 0) {
        QAbstractItemModel* oldModel = m_ui->remoteFiles->model();
        m_ui->remoteFiles->setModel(0, PROJECTS_IS_REMOTE);
        delete oldModel;
        oldModel = 0;
    }

    //
    // Установим новую модель
    //
    _remoteProjectsModel->setParent(m_ui->remoteFiles);
    m_ui->remoteFiles->setModel(_remoteProjectsModel, PROJECTS_IS_REMOTE);
}

void StartUpView::enableProgressLoginLabel(int _dots, bool _firstUpdate)
{
    QApplication::processEvents();

    if (_firstUpdate) {
        m_isProcessLogin = true;
    }

    if (m_isProcessLogin) {
        m_ui->login->setText(tr("Connect") + QString(".").repeated(_dots));
        QTimer::singleShot(1000, Qt::VeryCoarseTimer, [this, _dots] {
            enableProgressLoginLabel((_dots + 1) % 4, false);
        });
    }
}

void StartUpView::disableProgressLoginLabel()
{
    m_isProcessLogin = false;
    m_ui->login->setText(tr("<a href=\"#\" style=\"color:#2b78da;\">Login</a>"));
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
    if (m_ui->localProjects->isChecked()) {
        m_ui->filesSouces->setCurrentWidget(m_ui->recentFilesPage);
    } else {
        m_ui->filesSouces->setCurrentWidget(m_ui->remoteFilesPage);
    }
}

void StartUpView::cabinetChangeVisibility()
{
    bool isVisible = m_ui->cabinetFrame->isVisible();
    if (!isVisible) {
        m_ui->cabinetFrame->show();
    }
    const bool FIX = true;
    Animation::slide(m_ui->cabinetFrame, AnimationDirection::FromTopToBottom, FIX, !FIX, !isVisible);
    if (isVisible) {
        QTimer::singleShot(300, m_ui->cabinetFrame, &QWidget::hide);
    }
}

void StartUpView::initView()
{
    QFont versionFont = m_ui->version->font();
#ifdef Q_OS_LINUX
    versionFont.setPointSize(10);
#elif defined Q_OS_MAC
    versionFont.setPointSize(12);
#endif
    m_ui->version->setFont(versionFont);
    m_ui->version_2->setFont(versionFont);

    m_ui->version->setText(QApplication::applicationVersion());

    m_ui->updateInfo->hide();

    m_ui->remoteProjects->hide();
    m_ui->cabinetFrame->hide();
    m_ui->userEmail->hide();

    m_ui->filesSouces->setCurrentWidget(m_ui->recentFilesPage);

    initIconsColor();
}

void StartUpView::initConnections()
{
    connect(m_ui->login, SIGNAL(linkActivated(QString)), this, SIGNAL(loginClicked()));
    connect(m_ui->userEmail, &QLabel::linkActivated, this, &StartUpView::cabinetChangeVisibility);
    connect(m_ui->updateInfo, &QLabel::linkActivated, this, &StartUpView::updateRequested);
    connect(m_ui->logout, &QPushButton::clicked, this, &StartUpView::logoutClicked);
    connect(m_ui->createProject, SIGNAL(clicked(bool)), this, SIGNAL(createProjectClicked()));
    connect(m_ui->openProject, SIGNAL(clicked(bool)), this, SIGNAL(openProjectClicked()));
    connect(m_ui->help, SIGNAL(clicked(bool)), this, SIGNAL(helpClicked()));

    connect(m_ui->userName, &AcceptebleLineEdit::textAccepted, this, &StartUpView::userNameChanged);
    connect(m_ui->changePassword, &QPushButton::clicked, this, &StartUpView::passwordChangeClicked);
    connect(m_ui->getSubscriptionInfo, &QToolButton::clicked, this, &StartUpView::getSubscriptionInfoClicked);
    connect(m_ui->renewSubscription, &QPushButton::clicked, this, &StartUpView::renewSubscriptionClicked);

    connect(m_ui->localProjects, SIGNAL(toggled(bool)), this, SLOT(aboutFilesSourceChanged()));
    connect(m_ui->recentFiles, &ProjectsList::clicked, this, &StartUpView::openRecentProjectClicked);
    connect(m_ui->recentFiles, &ProjectsList::hideRequested, this, &StartUpView::hideRecentProjectRequested);
    connect(m_ui->remoteFiles, &ProjectsList::clicked, this, &StartUpView::openRemoteProjectClicked);
    connect(m_ui->remoteFiles, &ProjectsList::editRequested, this, &StartUpView::editRemoteProjectRequested);
    connect(m_ui->remoteFiles, &ProjectsList::removeRequested, this, &StartUpView::removeRemoteProjectRequested);
    connect(m_ui->remoteFiles, &ProjectsList::shareRequested, this, &StartUpView::shareRemoteProjectRequested);
    connect(m_ui->remoteFiles, &ProjectsList::unshareRequested, this, &StartUpView::unshareRemoteProjectRequested);
    connect(m_ui->refreshProjects, SIGNAL(clicked()), this, SIGNAL(refreshProjects()));
}

void StartUpView::initStyleSheet()
{
    m_ui->topEmptyLabel->setProperty("inTopPanel", true);
    m_ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
    m_ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

    m_ui->mainContainer->setProperty("mainContainer", true);
    m_ui->projectsFrame->setProperty("mainContainer", true);
    m_ui->projectsFrame->setProperty("baseForeground", true);

    m_ui->userName->setProperty("editableLabel", true);

    m_ui->getSubscriptionInfo->setProperty("leftAlignedText", true);
    m_ui->getSubscriptionInfo->setProperty("isUpdateButton", true);
    m_ui->renewSubscription->setProperty("leftAlignedText", true);
    m_ui->changePassword->setProperty("leftAlignedText", true);
    m_ui->logout->setProperty("leftAlignedText", true);
    m_ui->createProject->setProperty("leftAlignedText", true);
    m_ui->openProject->setProperty("leftAlignedText", true);
    m_ui->help->setProperty("leftAlignedText", true);

    m_ui->localProjects->setProperty("inStartUpView", true);
    m_ui->remoteProjects->setProperty("inStartUpView", true);
    m_ui->refreshProjects->setProperty("isUpdateButton", true);

    m_ui->recentFiles->setProperty("nobordersContainer", true);
    m_ui->recentFiles->viewport()->setStyleSheet("#ProjectListsContent { background-color: palette(base); }");
    m_ui->remoteFiles->setProperty("nobordersContainer", true);
    m_ui->remoteFiles->viewport()->setStyleSheet("#ProjectListsContent { background-color: palette(base); }");
}

void StartUpView::initIconsColor()
{
    const QSize iconSize = m_ui->createProject->iconSize();

    QIcon account(*m_ui->loginIcon->pixmap());
    ImageHelper::setIconColor(account, iconSize, palette().text().color());
    m_ui->loginIcon->setPixmap(account.pixmap(iconSize));

    QIcon createProject = m_ui->createProject->icon();
    ImageHelper::setIconColor(createProject, iconSize, palette().text().color());
    m_ui->createProject->setIcon(createProject);

    QIcon openProject = m_ui->openProject->icon();
    ImageHelper::setIconColor(openProject, iconSize, palette().text().color());
    m_ui->openProject->setIcon(openProject);

    QIcon help = m_ui->help->icon();
    ImageHelper::setIconColor(help, iconSize, palette().text().color());
    m_ui->help->setIcon(help);

    QIcon refresh = m_ui->refreshProjects->icon();
    ImageHelper::setIconColor(refresh, iconSize, palette().text().color());
    m_ui->refreshProjects->setIcon(refresh);
    m_ui->getSubscriptionInfo->setIcon(refresh);
}
