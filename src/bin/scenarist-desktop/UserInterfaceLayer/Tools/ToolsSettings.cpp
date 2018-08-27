#include "ToolsSettings.h"
#include "ui_ToolsSettings.h"

#include <Domain/ScriptVersion.h>

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Styles/TreeViewProxyStyle/TreeViewProxyStyle.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>

using UserInterface::ToolsSettings;

namespace {
    const int kInvalidScriptVersionIndex = -1;
}


ToolsSettings::ToolsSettings(QWidget *_parent) :
    QWidget(_parent),
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

void ToolsSettings::setScriptVersionsModel(QAbstractItemModel* _model)
{
    QSignalBlocker versionsFirstBlocker(m_ui->compareVersionFirst);
    QSignalBlocker versionsSecondBlocker(m_ui->compareVersionSecond);

    m_ui->compareVersionFirst->setModel(_model);
    m_ui->compareVersionFirst->setModelColumn(Domain::ScriptVersionsTable::kName);
    m_ui->compareVersionFirst->setCurrentIndex(kInvalidScriptVersionIndex);
    m_ui->compareVersionSecond->setModel(_model);
    m_ui->compareVersionSecond->setModelColumn(Domain::ScriptVersionsTable::kName);
    m_ui->compareVersionSecond->setCurrentIndex(kInvalidScriptVersionIndex);

    m_loadingIndicator->hide();
}

void ToolsSettings::initView()
{
    m_ui->back->setIcons(QIcon(":/Graphics/Iconset/arrow-left.svg"));
    m_ui->back->setToolTip(tr("Back to the tools list"));

    m_loadingIndicator->hide();

    m_ui->backups->setItemDelegate(new TreeViewItemDelegate(m_ui->backups));
    m_ui->backups->setStyle(new TreeViewProxyStyle(m_ui->backups->style()));
}

void ToolsSettings::initConnections()
{
    connect(m_ui->back, &FlatButton::clicked, this, &ToolsSettings::backPressed);
    //
    connect(m_ui->backups, &QListView::activated, this, &ToolsSettings::backupSelected);
    connect(m_ui->backups, &QListView::clicked, this, &ToolsSettings::backupSelected);
    //
    auto requestCompareVersions = [this] {
        const int firstVersionIndex = m_ui->compareVersionFirst->currentIndex();
        const int secondVersionIndex = m_ui->compareVersionSecond->currentIndex();
        if (firstVersionIndex == kInvalidScriptVersionIndex
            || secondVersionIndex == kInvalidScriptVersionIndex) {
            return;
        }

        emit versionsForCompareSelected(firstVersionIndex, secondVersionIndex);
    };
    connect(m_ui->compareVersionFirst, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, requestCompareVersions);
    connect(m_ui->compareVersionSecond, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, requestCompareVersions);
}

void ToolsSettings::initStyleSheet()
{
    m_ui->topEmptyLabel->setProperty("inTopPanel", true);
    m_ui->topEmptyLabel->setProperty("topPanelTopBordered", true);

    m_ui->back->setProperty("inTopPanel", true);

    m_ui->backups->setProperty("mainContainer", true);
    m_ui->scriptComparePage->setProperty("mainContainer", true);
}
