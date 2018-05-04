#include "StartUpView.h"
#include "ui_StartUpView.h"

#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>

#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

#include <UserInterfaceLayer/Account/ChangePasswordDialog.h>
#include <UserInterfaceLayer/Account/RenewSubscriptionDialog.h>

using UserInterface::StartUpView;
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
    // Установим новую модель
    //
    m_ui->recentFiles->setModel(_recentProjectsModel, PROJECTS_IS_LOCAL);
}

void StartUpView::setRecentProjectName(int _index, const QString& _name)
{
    m_ui->recentFiles->setProjectName(_index, _name);
}

void StartUpView::setUpdateInfo(const QString& _updateInfo)
{
    m_ui->updateInfo->setText(_updateInfo);
    m_ui->updateInfo->show();
}

void StartUpView::setRemoteProjectsVisible(bool _visible)
{
    m_ui->remoteProjects->setVisible(_visible);

    if (!_visible && m_ui->remoteProjects->isChecked()) {
        m_ui->localProjects->setChecked(true);
    }
}

void StartUpView::setRemoteProjects(QAbstractItemModel* _remoteProjectsModel)
{
    //
    // Установим новую модель
    //
    m_ui->remoteFiles->setModel(_remoteProjectsModel, PROJECTS_IS_REMOTE);
}

void StartUpView::setRemoteProjectName(int _index, const QString& _name)
{
    m_ui->remoteFiles->setProjectName(_index, _name);
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
        m_ui->filesSources->setCurrentWidget(m_ui->recentFilesPage);
    } else {
        m_ui->filesSources->setCurrentWidget(m_ui->remoteFilesPage);
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

    m_ui->filesSources->setCurrentWidget(m_ui->recentFilesPage);

    initIconsColor();
}

void StartUpView::initConnections()
{
    connect(m_ui->updateInfo, &QLabel::linkActivated, this, &StartUpView::updateRequested);
    connect(m_ui->createProject, SIGNAL(clicked(bool)), this, SIGNAL(createProjectClicked()));
    connect(m_ui->openProject, SIGNAL(clicked(bool)), this, SIGNAL(openProjectClicked()));
    connect(m_ui->help, SIGNAL(clicked(bool)), this, SIGNAL(helpClicked()));

    connect(m_ui->localProjects, SIGNAL(toggled(bool)), this, SLOT(aboutFilesSourceChanged()));
    connect(m_ui->recentFiles, &ProjectsList::clicked, this, &StartUpView::openRecentProjectClicked);
    connect(m_ui->recentFiles, &ProjectsList::hideRequested, this, &StartUpView::hideRecentProjectRequested);
    connect(m_ui->recentFiles, &ProjectsList::moveToCloudRequested, this, &StartUpView::moveToCloudRecentProjectRequested);
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
    m_ui->mainContainer->setProperty("baseForeground", true);
    m_ui->projectsFrame->setProperty("mainContainer", true);
    m_ui->projectsFrame->setProperty("windowForeground", true);

    m_ui->createProject->setProperty("leftAlignedText", true);
    m_ui->openProject->setProperty("leftAlignedText", true);
    m_ui->help->setProperty("leftAlignedText", true);

    m_ui->localProjects->setProperty("inStartUpView", true);
    m_ui->remoteProjects->setProperty("inStartUpView", true);
    m_ui->refreshProjects->setProperty("isUpdateButton", true);

    m_ui->recentFiles->setProperty("nobordersContainer", true);
    m_ui->recentFiles->viewport()->setStyleSheet("#ProjectListsContent { background-color: palette(window); }");
    m_ui->remoteFiles->setProperty("nobordersContainer", true);
    m_ui->remoteFiles->viewport()->setStyleSheet("#ProjectListsContent { background-color: palette(window); }");
}

void StartUpView::initIconsColor()
{
    const QSize iconSize = m_ui->createProject->iconSize();

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
}
