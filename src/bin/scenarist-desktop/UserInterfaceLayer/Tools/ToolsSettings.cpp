#include "ToolsSettings.h"
#include "ui_ToolsSettings.h"

#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>

using UserInterface::ToolsSettings;

namespace {

}


ToolsSettings::ToolsSettings(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ToolsSettings)
{
    m_ui->setupUi(this);
    m_loadingIndicator = new QLightBoxProgress(m_ui->content, false);

    initView();
    initConnections();
    initStyleSheet();
}

ToolsSettings::~ToolsSettings()
{
    delete m_ui;
}

void ToolsSettings::setTitle(const QString& _title)
{
    m_ui->topEmptyLabel->setText(_title);
}

void ToolsSettings::setCurrentType(int _index)
{
    QWidget* currentPropertiesPane = m_ui->content->widget(_index);
    m_ui->content->setCurrentWidget(currentPropertiesPane);
    m_loadingIndicator->showProgress(QString(), QString());
}

void ToolsSettings::setBackupsModel(QAbstractItemModel* _model)
{
    m_ui->backups->setModel(_model);
    m_loadingIndicator->hide();
}

void ToolsSettings::initView()
{
    m_ui->back->setIcons(QIcon(":/Graphics/Iconset/arrow-left.svg"));
    m_ui->back->setToolTip(tr("Back to the tools list"));

    m_loadingIndicator->hide();
}

void ToolsSettings::initConnections()
{
    connect(m_ui->back, &FlatButton::clicked, this, &ToolsSettings::backPressed);
    connect(m_ui->backups, &QListView::activated, this, &ToolsSettings::backupSelected);
    connect(m_ui->backups, &QListView::clicked, this, &ToolsSettings::backupSelected);
}

void ToolsSettings::initStyleSheet()
{
    m_ui->topEmptyLabel->setProperty("inTopPanel", true);
    m_ui->topEmptyLabel->setProperty("topPanelTopBordered", true);

    m_ui->back->setProperty("inTopPanel", true);
}
