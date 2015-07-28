#include "StatisticsView.h"

#include "ReportButton.h"
#include "StatisticsSettings.h"

#include <3rd_party/Widgets/Ctk/ctkCollapsibleButton.h>
#include <3rd_party/Widgets/Ctk/ctkPopupWidget.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>

#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QSplitter>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QVariant>
#include <QVBoxLayout>

using UserInterface::StatisticsView;
using UserInterface::StatisticsSettings;
using UserInterface::ReportButton;
using BusinessLogic::ReportParameters;


StatisticsView::StatisticsView(QWidget* _parent) :
	QWidget(_parent),
	m_leftTopEmptyLabel(new QLabel(this)),
	m_rightTopEmptyLabel(new QLabel(this)),
	m_settings(new FlatButton(this)),
	m_print(new FlatButton(this)),
	m_save(new FlatButton(this)),
	m_statisticTypes(new QFrame(this)),
	m_statisticSettings(new StatisticsSettings(this)),
	m_statisticData(new QStackedWidget(this)),
	m_reportData(new QTextBrowser(this)),
	m_progress(new ProgressWidget(m_statisticData, false))
{
	initView();
	initConnections();
	initStyleSheet();
}

void StatisticsView::setCharacters(QAbstractItemModel* _characters)
{
	m_statisticSettings->setCharacters(_characters);
}

void StatisticsView::setReport(const QString& _html)
{
	m_reportData->setHtml(_html);
}

void StatisticsView::showProgress()
{
	m_progress->showProgress(tr("Preparing report"), tr("Please wait. Preparing report to preview can take few minutes."));
	QApplication::processEvents();
}

void StatisticsView::hideProgress()
{
	m_progress->finish();
}

void StatisticsView::aboutInitDataPanel()
{
	if (ReportButton* button = qobject_cast<ReportButton*>(sender())) {
		if (button->type() == BusinessLogic::ReportParameters::Report) {
			m_statisticData->setCurrentWidget(m_reportData);
		} else {
			//
			// TODO: график
			//
		}

		m_statisticSettings->setCurrentIndex(button->group()->checkedId());

		if (ctkPopupWidget* settingsPanel = m_settings->findChild<ctkPopupWidget*>()) {
			settingsPanel->setFixedSize(m_statisticSettings->currentWidget()->sizeHint());
		}
	}
}

void StatisticsView::aboutMakeReport()
{
	if (ReportButton* button = qobject_cast<ReportButton*>(m_reports.first()->group()->checkedButton())) {
		BusinessLogic::ReportParameters parameters = m_statisticSettings->settings();
		parameters.type = button->type();
		if (parameters.type == BusinessLogic::ReportParameters::Report) {
			parameters.reportType = (BusinessLogic::ReportParameters::ReportType)button->subtype();
		} else {
			//
			// TODO: график
			//
		}

		emit makeReport(parameters);
	}
}

void StatisticsView::initView()
{
	//
	// Настраиваем панели инструментов
	//
	m_settings->setIcons(QIcon(":/Graphics/Icons/settings_tool.png"));
	m_settings->setCheckable(true);
	ctkPopupWidget* settingsPanel = new ctkPopupWidget(m_settings);
	QHBoxLayout* settingsPanelLayout = new QHBoxLayout(settingsPanel);
	settingsPanelLayout->setContentsMargins(QMargins());
	settingsPanelLayout->setSpacing(0);
	settingsPanelLayout->addWidget(m_statisticSettings);
	settingsPanel->setAutoShow(false);
	settingsPanel->setAutoHide(false);
	settingsPanel->setFixedSize(1, 0);
	connect(m_settings, SIGNAL(toggled(bool)), settingsPanel, SLOT(showPopup(bool)));

	m_print->setIcons(QIcon(":/Graphics/Icons/printer.png"));
	m_save->setIcons(QIcon(":/Graphics/Icons/Editing/download.png"));


	//
	// Настраиваем панель со списком отчётов
	//
	ctkCollapsibleButton* reports = new ctkCollapsibleButton(tr("Reports"), this);
	reports->setIndicatorAlignment(Qt::AlignRight);
	reports->setProperty("reportButton", true);

	m_reports << new ReportButton(tr("Statistics report"), ReportParameters::Report, ReportParameters::SummaryReport, reports);
	m_reports << new ReportButton(tr("Scene report"), ReportParameters::Report, ReportParameters::SceneReport, reports);
	m_reports << new ReportButton(tr("Location report"), ReportParameters::Report, ReportParameters::LocationReport, reports);
	m_reports << new ReportButton(tr("Cast report"), ReportParameters::Report, ReportParameters::CastReport, reports);
	m_reports << new ReportButton(tr("Character report"), ReportParameters::Report, ReportParameters::CharacterReport, reports);

	QVBoxLayout* reportsLayout = new QVBoxLayout;
	reportsLayout->setContentsMargins(QMargins());
	reportsLayout->setSpacing(0);
	foreach (ReportButton* button, m_reports) {
		reportsLayout->addWidget(button);
	}
	reports->setLayout(reportsLayout);

	QButtonGroup* group = new QButtonGroup(this);
	int buttonId = 1;
	foreach (ReportButton* button, m_reports) {
		group->addButton(button, buttonId++);
	}

	QVBoxLayout* statisticTypesLayout = new QVBoxLayout;
	statisticTypesLayout->setContentsMargins(QMargins());
	statisticTypesLayout->setSpacing(0);
	statisticTypesLayout->addWidget(reports);
	statisticTypesLayout->addStretch();
	m_statisticTypes->setLayout(statisticTypesLayout);

	QVBoxLayout* statisticTypesMainLayout = new QVBoxLayout;
	statisticTypesMainLayout->setContentsMargins(QMargins());
	statisticTypesMainLayout->setSpacing(0);
	statisticTypesMainLayout->addWidget(m_leftTopEmptyLabel);
	statisticTypesMainLayout->addWidget(m_statisticTypes);

	QWidget* statisticTypesPanel = new QWidget(this);
	statisticTypesPanel->setLayout(statisticTypesMainLayout);


	//
	// Настраиваем панель с данными по отчётам
	//
	m_statisticData->addWidget(m_reportData);

	QHBoxLayout* toolbarLayout = new QHBoxLayout;
	toolbarLayout->setContentsMargins(QMargins());
	toolbarLayout->setSpacing(0);
	toolbarLayout->addWidget(m_settings);
	toolbarLayout->addWidget(m_print);
	toolbarLayout->addWidget(m_save);
	toolbarLayout->addWidget(m_rightTopEmptyLabel);

	QVBoxLayout* statisticDataLayout = new QVBoxLayout;
	statisticDataLayout->setContentsMargins(QMargins());
	statisticDataLayout->setSpacing(0);
	statisticDataLayout->addLayout(toolbarLayout);
	statisticDataLayout->addWidget(m_statisticData, 1);

	QWidget* statisticDataPanel = new QWidget(this);
	statisticDataPanel->setLayout(statisticDataLayout);


	//
	// Объединяем всё
	//
	QSplitter* splitter = new QSplitter(this);
	splitter->setHandleWidth(1);
	splitter->setOpaqueResize(false);
	splitter->addWidget(statisticTypesPanel);
	splitter->addWidget(statisticDataPanel);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(splitter);
	setLayout(layout);
}

void StatisticsView::initConnections()
{
	foreach (ReportButton* button, m_reports) {
		connect(button, SIGNAL(clicked(bool)), this, SLOT(aboutInitDataPanel()));
		connect(button, SIGNAL(clicked(bool)), this, SLOT(aboutMakeReport()));
	}

	connect(m_statisticSettings, SIGNAL(settingsChanged()), this, SLOT(aboutMakeReport()));
}

void StatisticsView::initStyleSheet()
{
	m_leftTopEmptyLabel->setProperty("inTopPanel", true);
	m_leftTopEmptyLabel->setProperty("topPanelTopBordered", true);

	m_settings->setProperty("inTopPanel", true);
	m_print->setProperty("inTopPanel", true);
	m_save->setProperty("inTopPanel", true);

	m_rightTopEmptyLabel->setProperty("inTopPanel", true);
	m_rightTopEmptyLabel->setProperty("topPanelTopBordered", true);
	m_rightTopEmptyLabel->setProperty("topPanelRightBordered", true);

	m_statisticTypes->setProperty("mainContainer", true);
	m_statisticData->setProperty("mainContainer", true);
}

