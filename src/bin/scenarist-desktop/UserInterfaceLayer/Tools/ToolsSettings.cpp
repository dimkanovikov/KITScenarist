#include "ToolsSettings.h"
#include "ui_ToolsSettings.h"

#include <3rd_party/Widgets/FlatButton/FlatButton.h>

using UserInterface::ToolsSettings;


ToolsSettings::ToolsSettings(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ToolsSettings)
{
    m_ui->setupUi(this);

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
    m_ui->content->setCurrentIndex(_index);
}

void ToolsSettings::initView()
{
    m_ui->back->setIcons(QIcon(":/Graphics/Iconset/arrow-left.svg"));
    m_ui->back->setToolTip(tr("Back to the tools list"));
}

void ToolsSettings::initConnections()
{
    connect(m_ui->back, &FlatButton::clicked, this, &ToolsSettings::backPressed);
}

void ToolsSettings::initStyleSheet()
{
    m_ui->topEmptyLabel->setProperty("inTopPanel", true);
    m_ui->topEmptyLabel->setProperty("topPanelTopBordered", true);

    m_ui->back->setProperty("inTopPanel", true);
}
