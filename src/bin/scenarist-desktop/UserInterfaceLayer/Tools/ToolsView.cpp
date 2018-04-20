#include "ToolsView.h"
#include "ToolsSettings.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h>

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Styles/TreeViewProxyStyle/TreeViewProxyStyle.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/ScalableWrapper/ScalableWrapper.h>
#include <3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.h>

#include <QLabel>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

using UserInterface::ScenarioTextEdit;
using UserInterface::ToolsSettings;
using UserInterface::ToolsView;


ToolsView::ToolsView(QWidget* _parent) :
    QWidget(_parent),
    m_leftTopEmptyLabel(new QLabel(this)),
    m_rightTopEmptyLabel(new QLabel(this)),
    m_settings(new FlatButton(this)),
    m_restore(new FlatButton(this)),
    m_navigation(new QStackedWidget(this)),
    m_toolsTypes(new QTreeWidget(this)),
    m_toolsSettings(new ToolsSettings(this)),
    m_content(new QStackedWidget(this)),
    m_placeholder(new QLabel(this)),
    m_editor(new ScenarioTextEdit(this)),
    m_editorWrapper(new ScalableWrapper(m_editor, this))
{
    initView();
    initConnections();
    initStyleSheet();
}

void ToolsView::reset()
{
    const int toolsTypesIndex = 0;
    m_navigation->setCurrentIndex(toolsTypesIndex);
    m_toolsTypes->clearSelection();

    m_restore->hide();

    showPlaceholderText(tr("Choose tool from list"));
}

void ToolsView::setShowScenesNumbers(bool _show)
{
    m_editor->setShowSceneNumbers(_show);
}

void ToolsView::setShowDialoguesNumbers(bool _show)
{
    m_editor->setShowDialoguesNumbers(_show);
}

void ToolsView::setTextEditColors(const QColor& _textColor, const QColor& _backgroundColor)
{
    m_editor->viewport()->setStyleSheet(QString("color: %1; background-color: %2;").arg(_textColor.name(), _backgroundColor.name()));
    m_editor->setStyleSheet(QString("#scenarioEditor { color: %1; }").arg(_textColor.name()));
}

void ToolsView::setTextEditZoomRange(qreal _zoomRange)
{
    m_editorWrapper->setZoomRange(_zoomRange);
}

void ToolsView::showPlaceholderText(const QString& _text)
{
    m_restore->hide();
    m_placeholder->setText(_text);
    WAF::StackedWidgetAnimation::fadeIn(m_content, m_placeholder);
}

void ToolsView::showScript()
{
    m_restore->show();
    m_editor->relayoutDocument();
    WAF::StackedWidgetAnimation::fadeIn(m_content, m_editorWrapper);
}

void ToolsView::setScriptDocument(BusinessLogic::ScenarioTextDocument* _document)
{
    m_editor->setScenarioDocument(_document);
}

void ToolsView::setBackupsModel(QAbstractItemModel* _model)
{
    m_toolsSettings->setBackupsModel(_model);

    if (_model == nullptr) {
        showPlaceholderText(tr("Didn't find any backups for the current project"));
    }
}

void ToolsView::activateTool(QTreeWidgetItem* _toolItem)
{
    m_toolsSettings->setTitle(_toolItem->text(0));
    m_toolsSettings->setCurrentType(m_toolsTypes->indexOfTopLevelItem(_toolItem));
    QTimer::singleShot(0, m_settings, &FlatButton::click);
}

void ToolsView::initView()
{
    m_leftTopEmptyLabel->setText(tr("Tools"));

    m_settings->setIcons(QIcon(":/Graphics/Iconset/settings.svg"));
    m_settings->setToolTip(tr("Tool settings"));
    m_restore->setIcons(QIcon(":/Graphics/Iconset/check.svg"));
    m_restore->setToolTip(tr("Restore script"));

    //
    // Настраиваем панель со списком инструментов
    //
    QTreeWidgetItem* versions = new QTreeWidgetItem(m_toolsTypes, { tr("Compare script versions") });
    versions->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/wrench.svg"));
    m_toolsTypes->addTopLevelItem(versions);
    QTreeWidgetItem* backups = new QTreeWidgetItem(m_toolsTypes, { tr("Restore script from backup") });
    backups->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/wrench.svg"));
    m_toolsTypes->addTopLevelItem(backups);

    m_toolsTypes->setObjectName("ToolsTypesTree");
    m_toolsTypes->expandAll();
    m_toolsTypes->setHeaderHidden(true);
    m_toolsTypes->setItemDelegate(new TreeViewItemDelegate(m_toolsTypes));
    m_toolsTypes->setStyle(new TreeViewProxyStyle(m_toolsTypes->style()));

    m_editor->setReadOnly(true);

    //
    // Настраиваем общую панель с группами отчётов
    //
    QHBoxLayout* toolsToolbarLayout = new QHBoxLayout;
    toolsToolbarLayout->setContentsMargins(QMargins());
    toolsToolbarLayout->setSpacing(0);
    toolsToolbarLayout->addWidget(m_leftTopEmptyLabel);
    toolsToolbarLayout->addWidget(m_settings);
    //
    QVBoxLayout* toolsLayout = new QVBoxLayout;
    toolsLayout->setContentsMargins(QMargins());
    toolsLayout->setSpacing(0);
    toolsLayout->addLayout(toolsToolbarLayout);
    toolsLayout->addWidget(m_toolsTypes);

    //
    // Настраиваем виджет навигации целиком
    //
    QWidget* toolsPanel = new QWidget(this);
    toolsPanel->setObjectName("toolsTypesPanel");
    toolsPanel->setLayout(toolsLayout);
    m_navigation->addWidget(toolsPanel);
    m_navigation->addWidget(m_toolsSettings);

    //
    // Настраиваем панель с результатами работы инструментов
    //
    m_placeholder->setAlignment(Qt::AlignCenter);

    m_editor->setPageMargins(QMarginsF{15, 5, 12, 5});
    m_editor->setUseSpellChecker(false);

    QHBoxLayout* toolDataToolbarLayout = new QHBoxLayout;
    toolDataToolbarLayout->setContentsMargins(QMargins());
    toolDataToolbarLayout->setSpacing(0);
    toolDataToolbarLayout->addWidget(m_restore);
    toolDataToolbarLayout->addWidget(m_rightTopEmptyLabel);
    //
    m_content->addWidget(m_placeholder);
    m_content->addWidget(m_editorWrapper);
    //
    QVBoxLayout* toolDataLayout = new QVBoxLayout;
    toolDataLayout->setContentsMargins(QMargins());
    toolDataLayout->setSpacing(0);
    toolDataLayout->addLayout(toolDataToolbarLayout);
    toolDataLayout->addWidget(m_content, 1);

    //
    // Настраиваем виджет результатов работы инструментов целиком
    //
    QWidget* toolDataPanel = new QWidget(this);
    toolDataPanel->setObjectName("toolDataPanel");
    toolDataPanel->setLayout(toolDataLayout);


    //
    // Объединяем всё
    //
    QSplitter* splitter = new QSplitter(this);
    splitter->setObjectName("toolsSplitter");
    splitter->setHandleWidth(1);
    splitter->setOpaqueResize(false);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(m_navigation);
    splitter->addWidget(toolDataPanel);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(splitter);
    setLayout(layout);
}

void ToolsView::initConnections()
{
    connect(m_toolsTypes, &QTreeWidget::itemActivated, this, &ToolsView::activateTool);
    connect(m_toolsTypes, &QTreeWidget::itemClicked, this, &ToolsView::activateTool);
    connect(m_settings, &FlatButton::clicked, [this] {
        const int delay = WAF::StackedWidgetAnimation::slide(m_navigation, m_toolsSettings, WAF::FromRightToLeft);
        QTimer::singleShot(delay, [this] { emit dataRequested(m_toolsTypes->currentIndex().row()); });
    });
    connect(m_toolsSettings, &ToolsSettings::backPressed, [this] { WAF::StackedWidgetAnimation::slide(m_navigation, m_navigation->widget(0), WAF::FromLeftToRight); });
    connect(m_toolsSettings, &ToolsSettings::backupSelected, this, &ToolsView::backupSelected);
    connect(m_restore, &FlatButton::clicked, this, &ToolsView::applyScriptRequested);
}

void ToolsView::initStyleSheet()
{
    m_leftTopEmptyLabel->setProperty("inTopPanel", true);
    m_leftTopEmptyLabel->setProperty("topPanelTopBordered", true);

    m_settings->setProperty("inTopPanel", true);

    m_rightTopEmptyLabel->setProperty("inTopPanel", true);
    m_rightTopEmptyLabel->setProperty("topPanelTopBordered", true);
    m_rightTopEmptyLabel->setProperty("topPanelRightBordered", true);

    m_restore->setProperty("inTopPanel", true);

    m_toolsTypes->setProperty("mainContainer", true);
    m_editorWrapper->setProperty("mainContainer", true);
}
