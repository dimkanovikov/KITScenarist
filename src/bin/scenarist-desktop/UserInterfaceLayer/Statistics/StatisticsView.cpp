#include "StatisticsView.h"

#include "ReportButton.h"
#include "StatisticsSettings.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/Statistics/Plots/AbstractPlot.h>

#include <3rd_party/Widgets/Ctk/ctkCollapsibleButton.h>
#include <3rd_party/Widgets/Ctk/ctkPopupWidget.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Widgets/QCutomPlot/qcustomplot.h>

#include <QApplication>
#include <QButtonGroup>
#include <QFileInfo>
#include <QFileDialog>
#include <QFrame>
#include <QLabel>
#include <QPageLayout>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTextBrowser>
#include <QVariant>
#include <QVBoxLayout>

using UserInterface::StatisticsView;
using UserInterface::StatisticsSettings;
using UserInterface::ReportButton;
using BusinessLogic::StatisticsParameters;

namespace {
	/**
	 * @brief Получить путь к последней используемой папке
	 */
	static QString reportsFolderPath() {
		QString reportsFolderPath =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"reports/save-folder",
					DataStorageLayer::SettingsStorage::ApplicationSettings);
		if (reportsFolderPath.isEmpty()) {
			reportsFolderPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		}
		return reportsFolderPath;
	}

	/**
	 * @brief Получить путь к сохраняемому файлу
	 */
	static QString reportFilePath(const QString& _fileName) {
		QString filePath = reportsFolderPath() + QDir::separator() + _fileName;
		return QDir::toNativeSeparators(filePath);
	}

	/**
	 * @brief Сохранить путь к последней используемой папке
	 */
	static void saveReportsFolderPath(const QString& _path) {
		DataStorageLayer::StorageFacade::settingsStorage()->setValue(
					"reports/save-folder",
					QFileInfo(_path).absoluteDir().absolutePath(),
					DataStorageLayer::SettingsStorage::ApplicationSettings);
	}
}


StatisticsView::StatisticsView(QWidget* _parent) :
	QWidget(_parent),
	m_leftTopEmptyLabel(new QLabel(this)),
	m_rightTopEmptyLabel(new QLabel(this)),
	m_settings(new FlatButton(this)),
	m_print(new FlatButton(this)),
	m_save(new FlatButton(this)),
	m_update(new FlatButton(this)),
	m_statisticTypes(new QFrame(this)),
	m_statisticSettings(new StatisticsSettings(this)),
	m_statisticData(new QStackedWidget(this)),
	m_reportData(new QTextBrowser(this)),
	m_plotData(new QCustomPlot(this)),
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

void StatisticsView::setPlot(const QVector<BusinessLogic::PlotData>& _plotData)
{
	m_plotData->clearGraphs();

	int plotIndex = 0;
	foreach (const BusinessLogic::PlotData& singlePlotData, _plotData) {
		//
		// Добавляем график и настраиваем его
		//
		m_plotData->addGraph();
		m_plotData->graph(plotIndex)->setName(singlePlotData.name);
		m_plotData->graph(plotIndex)->setPen(QPen(singlePlotData.color, 2));

		//
		// Отправляем данные в график
		//
		m_plotData->graph(plotIndex)->setData(singlePlotData.x, singlePlotData.y);

		plotIndex += 1;
	}

	//
	// Позволим графику самому масштабироваться для лучшего вида
	//
	for (plotIndex = 0; plotIndex < m_plotData->graphCount(); ++plotIndex) {
		m_plotData->graph(plotIndex)->rescaleAxes(plotIndex > 0 ? true : false);
	}

	m_plotData->replot();
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
		if (button->type() == BusinessLogic::StatisticsParameters::Report) {
			m_statisticData->setCurrentWidget(m_reportData);
		} else {
			m_statisticData->setCurrentWidget(m_plotData);
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
		BusinessLogic::StatisticsParameters parameters = m_statisticSettings->settings();
		parameters.type = button->type();
		if (parameters.type == BusinessLogic::StatisticsParameters::Report) {
			parameters.reportType = (BusinessLogic::StatisticsParameters::ReportType)button->subtype();
		} else {
			parameters.plotType = (BusinessLogic::StatisticsParameters::PlotType)button->subtype();
		}

		emit makeReport(parameters);
	}
}

void StatisticsView::aboutPrintReport()
{
	QPrinter printer;
	printer.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);
	QPrintPreviewDialog printDialog(&printer, this);
	printDialog.setWindowState(Qt::WindowMaximized);
	connect(&printDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(aboutPrint(QPrinter*)));

	//
	// Вызываем диалог предварительного просмотра и печати
	//
	printDialog.exec();
}

void StatisticsView::aboutPrint(QPrinter* _printer)
{
	m_reportData->document()->print(_printer);
}

void StatisticsView::aboutSaveReport()
{
	const QString saveFileName = ::reportFilePath(tr("Report.pdf"));
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save report"), saveFileName, tr("PDF files (*.pdf)"));
	if (!fileName.isEmpty()) {
		if (!fileName.endsWith(".pdf")) {
			fileName.append(".pdf");
		}
		QPrinter printer;
		printer.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);
		m_reportData->print(&printer);

		::saveReportsFolderPath(fileName);
	}
}

void StatisticsView::initView()
{
	//
	// Настраиваем панели инструментов
	//
	m_settings->setIcons(QIcon(":/Graphics/Icons/settings_tool.png"));
	m_settings->setCheckable(true);
	m_settings->setToolTip(tr("Report settings"));
	ctkPopupWidget* settingsPanel = new ctkPopupWidget(m_settings);
	QHBoxLayout* settingsPanelLayout = new QHBoxLayout(settingsPanel);
	settingsPanelLayout->setContentsMargins(QMargins());
	settingsPanelLayout->setSpacing(0);
	settingsPanelLayout->addWidget(m_statisticSettings);
	settingsPanel->setAutoShow(false);
	settingsPanel->setAutoHide(false);
	settingsPanel->setEffectDuration(200);
	settingsPanel->setFixedSize(1, 0);
	connect(m_settings, SIGNAL(toggled(bool)), settingsPanel, SLOT(showPopup(bool)));

	m_print->setIcons(QIcon(":/Graphics/Icons/printer.png"));
	m_print->setToolTip(tr("Print preview"));
	m_save->setIcons(QIcon(":/Graphics/Icons/Editing/download.png"));
	m_save->setToolTip(tr("Save report to file"));
	m_update->setIcons(QIcon(":/Graphics/Icons/Editing/refresh.png"));
	m_update->setToolTip(tr("Update current report"));


	//
	// Настраиваем панель со списком отчётов
	//
	ctkCollapsibleButton* reports = new ctkCollapsibleButton(tr("Reports"), this);
	reports->setIndicatorAlignment(Qt::AlignRight);
	reports->setProperty("reportButton", true);

	const int REPORTS_MIN = m_reports.size();
	m_reports << new ReportButton(tr("Summary statistics"), StatisticsParameters::Report, StatisticsParameters::SummaryReport, reports);
	m_reports << new ReportButton(tr("Scene report"), StatisticsParameters::Report, StatisticsParameters::SceneReport, reports);
	m_reports << new ReportButton(tr("Location report"), StatisticsParameters::Report, StatisticsParameters::LocationReport, reports);
	m_reports << new ReportButton(tr("Cast report"), StatisticsParameters::Report, StatisticsParameters::CastReport, reports);
	m_reports << new ReportButton(tr("Character dialogues"), StatisticsParameters::Report, StatisticsParameters::CharacterReport, reports);
	const int REPORTS_MAX = m_reports.size();

	QVBoxLayout* reportsLayout = new QVBoxLayout;
	reportsLayout->setContentsMargins(QMargins());
	reportsLayout->setSpacing(0);
	for (int reportIndex = REPORTS_MIN; reportIndex < REPORTS_MAX; ++reportIndex) {
		reportsLayout->addWidget(m_reports.at(reportIndex));
	}
	reports->setLayout(reportsLayout);

	//
	// Настраиваем панель со списком графиков
	//
	ctkCollapsibleButton* plots = new ctkCollapsibleButton(tr("Plots"), this);
	plots->setIndicatorAlignment(Qt::AlignRight);
	plots->setProperty("reportButton", true);

	const int PLOTS_MIN = m_reports.size();
	m_reports << new ReportButton(tr("Story structure analysis"), StatisticsParameters::Plot, StatisticsParameters::StoryStructureAnalisysPlot, plots);
	const int PLOTS_MAX = m_reports.size();

	QVBoxLayout* plotsLayout = new QVBoxLayout;
	plotsLayout->setContentsMargins(QMargins());
	plotsLayout->setSpacing(0);
	for (int plotIndex = PLOTS_MIN; plotIndex < PLOTS_MAX; ++plotIndex) {
		plotsLayout->addWidget(m_reports.at(plotIndex));
	}
	plots->setLayout(plotsLayout);

	//
	// Помещаем всех отчёты и графики в группу
	//
	QButtonGroup* reportsGroup = new QButtonGroup(reports);
	{
		int reportId = 1;
		for (int reportIndex = 0; reportIndex < m_reports.size(); ++reportIndex) {
			reportsGroup->addButton(m_reports.at(reportIndex), reportId++);
		}
	}

	//
	// Настраиваем общую панель с группами отчётов
	//
	QVBoxLayout* statisticTypesLayout = new QVBoxLayout;
	statisticTypesLayout->setContentsMargins(QMargins());
	statisticTypesLayout->setSpacing(0);
	statisticTypesLayout->addWidget(reports);
	statisticTypesLayout->addWidget(plots);
	statisticTypesLayout->addStretch();
	m_statisticTypes->setLayout(statisticTypesLayout);

	QVBoxLayout* statisticTypesMainLayout = new QVBoxLayout;
	statisticTypesMainLayout->setContentsMargins(QMargins());
	statisticTypesMainLayout->setSpacing(0);
	statisticTypesMainLayout->addWidget(m_leftTopEmptyLabel);
	statisticTypesMainLayout->addWidget(m_statisticTypes);

	QWidget* statisticTypesPanel = new QWidget(this);
	statisticTypesPanel->setObjectName("statisticTypesPanel");
	statisticTypesPanel->setLayout(statisticTypesMainLayout);

	//
	// Настраиваем контейнеры отчётов
	//
//	// configure right and top axis to show ticks but no labels:
//	// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
//	m_plotData->xAxis2->setVisible(true);
//	m_plotData->xAxis2->setTickLabels(false);
//	m_plotData->yAxis2->setVisible(true);
//	m_plotData->yAxis2->setTickLabels(false);
//	// make left and bottom axes always transfer their ranges to right and top axes:
//	connect(m_plotData->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plotData->xAxis2, SLOT(setRange(QCPRange)));
//	connect(m_plotData->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plotData->yAxis2, SLOT(setRange(QCPRange)));
	//
//	m_plotData->xAxis->setAutoTickLabels(true);
//	m_plotData->xAxis->setAutoTickStep(false);
//	m_plotData->xAxis->setAutoTickCount(20);
//	m_plotData->xAxis->setAutoSubTicks(false);
//	m_plotData->xAxis->setSubTickCount(0);
	// setup legend:
//	m_plotData->legend->setFont(QFont(font().family(), 7));
	m_plotData->legend->setIconSize(50, 20);
	m_plotData->legend->setVisible(true);
	// Note: we could have also just called customPlot->rescaleAxes(); instead
	// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
	m_plotData->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	m_plotData->axisRect()->setRangeZoom(Qt::Horizontal);
	m_plotData->axisRect()->setRangeDragAxes(m_plotData->xAxis, 0);

	//
	// Настраиваем панель с данными по отчётам
	//
	m_statisticData->addWidget(m_reportData);
	m_statisticData->addWidget(m_plotData);

	QHBoxLayout* toolbarLayout = new QHBoxLayout;
	toolbarLayout->setContentsMargins(QMargins());
	toolbarLayout->setSpacing(0);
	toolbarLayout->addWidget(m_settings);
	toolbarLayout->addWidget(m_print);
	toolbarLayout->addWidget(m_save);
	toolbarLayout->addWidget(m_update);
	toolbarLayout->addWidget(m_rightTopEmptyLabel);

	QVBoxLayout* statisticDataLayout = new QVBoxLayout;
	statisticDataLayout->setContentsMargins(QMargins());
	statisticDataLayout->setSpacing(0);
	statisticDataLayout->addLayout(toolbarLayout);
	statisticDataLayout->addWidget(m_statisticData, 1);

	QWidget* statisticDataPanel = new QWidget(this);
	statisticDataPanel->setObjectName("statisticDataPanel");
	statisticDataPanel->setLayout(statisticDataLayout);


	//
	// Объединяем всё
	//
	QSplitter* splitter = new QSplitter(this);
	splitter->setObjectName("statisticsSplitter");
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

	connect(m_print, SIGNAL(clicked(bool)), this, SLOT(aboutPrintReport()));
	connect(m_save, SIGNAL(clicked(bool)), this, SLOT(aboutSaveReport()));
	connect(m_update, &FlatButton::clicked, this, &StatisticsView::aboutMakeReport);
}

void StatisticsView::initStyleSheet()
{
	m_leftTopEmptyLabel->setProperty("inTopPanel", true);
	m_leftTopEmptyLabel->setProperty("topPanelTopBordered", true);

	m_settings->setProperty("inTopPanel", true);
	m_print->setProperty("inTopPanel", true);
	m_save->setProperty("inTopPanel", true);
	m_update->setProperty("inTopPanel", true);

	m_rightTopEmptyLabel->setProperty("inTopPanel", true);
	m_rightTopEmptyLabel->setProperty("topPanelTopBordered", true);
	m_rightTopEmptyLabel->setProperty("topPanelRightBordered", true);

	m_statisticTypes->setProperty("mainContainer", true);
	m_statisticData->setProperty("mainContainer", true);
}

