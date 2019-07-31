#include "StatisticsView.h"

#include "StatisticsSettings.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/Statistics/Plots/AbstractPlot.h>

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Styles/TreeViewProxyStyle/TreeViewProxyStyle.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Widgets/QCutomPlot/qcustomplotextended.h>
#include <3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.h>

#include <QApplication>
#include <QButtonGroup>
#include <QFileInfo>
#include <QFileDialog>
#include <QFrame>
#include <QLabel>
#include <QModelIndex>
#include <QPageLayout>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QVariant>
#include <QVBoxLayout>

#include <cmath>

using UserInterface::StatisticsView;
using UserInterface::StatisticsSettings;
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
    m_leftTopEmptyLabel(new QLabel(tr("Statistics"), this)),
    m_rightTopEmptyLabel(new QLabel(this)),
    m_settings(new FlatButton(this)),
    m_print(new FlatButton(this)),
    m_save(new FlatButton(this)),
    m_update(new FlatButton(this)),
    m_navigation(new QStackedWidget(this)),
    m_statisticTypes(new QTreeWidget(this)),
    m_statisticSettings(new StatisticsSettings(this)),
    m_statisticData(new QStackedWidget(this)),
    m_reportData(new QTextBrowser(this)),
    m_plotData(new QCustomPlotExtended(this)),
    m_progress(new QLightBoxProgress(m_statisticData, false))
{
    initView();
    initPlot();
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

void StatisticsView::setPlot(const BusinessLogic::Plot& _plot)
{
    //
    // Очищаем график и настраиваем цвета в соответствии с палитрой
    //
    m_plotData->clearGraphs();
    initPlot();

    //
    // Загружаем информацию и данные
    //
    m_plotData->setPlotInfo(_plot.info);
    qreal maxX = 0;
    qreal maxY = 0;
    for (const BusinessLogic::PlotData& singlePlotData : _plot.data) {
        //
        // Добавляем график и настраиваем его
        //
        QCPGraph* plot = m_plotData->addGraph();
        plot->setName(singlePlotData.name);
        plot->setPen(QPen(singlePlotData.color, 2));
        if (_plot.useBrush) {
            plot->setBrush(singlePlotData.color);
        }

        //
        // Отправляем данные в график
        //
        plot->setData(singlePlotData.x, singlePlotData.y);

        //
        // Определяем максимумы
        //
        for (const qreal& x : singlePlotData.x) {
            if (!std::isnan(x) && x > maxX) {
                maxX = x;
            }
        }
        for (const qreal& y : singlePlotData.y) {
            if (!std::isnan(y) && y > maxY) {
                maxY = y;
            }
        }
    }

    //
    // Масштабируем график
    //
    m_plotData->xAxis->setRangeLower(0);
    m_plotData->xAxis->setRangeUpper(maxX);
    m_plotData->yAxis->setRangeLower(0);
    m_plotData->yAxis->setRangeUpper(maxY + 1);

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

void StatisticsView::activateReport()
{
    if (m_statisticTypes->currentItem() == nullptr) {
        return;
    }

    const QTreeWidgetItem* item = m_statisticTypes->currentItem();
    if (!item->data(0, Qt::UserRole).isValid()
        || !item->data(0, Qt::UserRole + 1).isValid()) {
        m_settings->setEnabled(false);
        return;
    }

    m_settings->setEnabled(true);
    m_statisticSettings->setTitle(item->text(0));

    const int type = item->data(0, Qt::UserRole).toInt();
    if (type == BusinessLogic::StatisticsParameters::Report) {
        m_print->show();
        m_statisticData->setCurrentWidget(m_reportData);
    } else {
        m_print->hide();
        m_statisticData->setCurrentWidget(m_plotData);
    }

    //
    // TODO: переделать по феншую через тип отчёта/графика
    //
    const QModelIndex itemIndex = m_statisticTypes->currentIndex();
    const QModelIndex parentIndex = itemIndex.parent();
    const int settingsIndex = parentIndex.row() * 5 + itemIndex.row();
    m_statisticSettings->setCurrentType(settingsIndex);

    QTimer::singleShot(0, m_settings, &FlatButton::click);
}

void StatisticsView::makeReport()
{
    if (m_statisticTypes->currentItem() == nullptr) {
        return;
    }

    const QTreeWidgetItem* item = m_statisticTypes->currentItem();
    if (!item->data(0, Qt::UserRole).isValid()
        || !item->data(0, Qt::UserRole + 1).isValid()) {
        return;
    }

    const int type = item->data(0, Qt::UserRole).toInt();
    const int subtype = item->data(0, Qt::UserRole + 1).toInt();
    BusinessLogic::StatisticsParameters parameters = m_statisticSettings->settings();
    parameters.type = static_cast<BusinessLogic::StatisticsParameters::Type>(type);
    if (parameters.type == BusinessLogic::StatisticsParameters::Report) {
        parameters.reportType = static_cast<BusinessLogic::StatisticsParameters::ReportType>(subtype);
    } else {
        parameters.plotType = static_cast<BusinessLogic::StatisticsParameters::PlotType>(subtype);
    }

    emit makeReportRequested(parameters);
}

void StatisticsView::printReport()
{
    QPrinter printer;
    printer.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);
    QPrintPreviewDialog printDialog(&printer, this);
    printDialog.setWindowState(Qt::WindowMaximized);
    connect(&printDialog, &QPrintPreviewDialog::paintRequested, this, &StatisticsView::print);

    //
    // Вызываем диалог предварительного просмотра и печати
    //
    printDialog.exec();
}

void StatisticsView::print(QPrinter* _printer)
{
    m_reportData->document()->print(_printer);
}

void StatisticsView::saveReport()
{
    //
    // Сохраняем отчёт
    //
    if (m_statisticData->currentWidget() == m_reportData) {
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
            QTextDocument reportDocument;
            reportDocument.setHtml(m_reportData->toHtml()
                                   .remove(QRegularExpression("(<a.*?>|</a>|<span.*?>|</span>)")));
            reportDocument.print(&printer);

            ::saveReportsFolderPath(fileName);
        }
    }
    //
    // Сохраняем график
    //
    else {
        const QString saveFileName = ::reportFilePath(tr("Plot.png"));
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save plot"), saveFileName, tr("PNG files (*.png)"));
        if (!fileName.isEmpty()) {
            if (!fileName.endsWith(".png")) {
                fileName.append(".png");
            }
            m_plotData->savePng(fileName);

            ::saveReportsFolderPath(fileName);
        }
    }
}

void StatisticsView::initView()
{
    //
    // Настраиваем панели инструментов
    //
    m_settings->setIcons(QIcon(":/Graphics/Iconset/settings.svg"));
    m_settings->setToolTip(tr("Report settings"));
    m_settings->setEnabled(false);
    m_print->setIcons(QIcon(":/Graphics/Iconset/printer.svg"));
    m_print->setToolTip(tr("Print preview"));
    m_save->setIcons(QIcon(":/Graphics/Iconset/content-save.svg"));
    m_save->setToolTip(tr("Save report to file"));
    m_update->setIcons(QIcon(":/Graphics/Iconset/refresh.svg"));
    m_update->setToolTip(tr("Update current report"));
    m_navigation->setObjectName("statisticsNavigation");


    //
    // Настраиваем панель со списком отчётов
    //
    QTreeWidgetItem* reports = new QTreeWidgetItem(m_statisticTypes, { tr("Reports") });
    reports->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/clipboard-text.svg"));
    QTreeWidgetItem* summaryStatisticsReport = new QTreeWidgetItem(reports, { tr("Summary statistics") });
    summaryStatisticsReport->setData(0, Qt::UserRole, StatisticsParameters::Report);
    summaryStatisticsReport->setData(0, Qt::UserRole + 1, StatisticsParameters::SummaryReport);
    summaryStatisticsReport->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/file-document-box.svg"));
    QTreeWidgetItem* sceneReport = new QTreeWidgetItem(reports, { tr("Scene report") });
    sceneReport->setData(0, Qt::UserRole, StatisticsParameters::Report);
    sceneReport->setData(0, Qt::UserRole + 1, StatisticsParameters::SceneReport);
    sceneReport->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/file-document-box.svg"));
    QTreeWidgetItem* locationReport = new QTreeWidgetItem(reports, { tr("Location report") });
    locationReport->setData(0, Qt::UserRole, StatisticsParameters::Report);
    locationReport->setData(0, Qt::UserRole + 1, StatisticsParameters::LocationReport);
    locationReport->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/file-document-box.svg"));
    QTreeWidgetItem* castReport = new QTreeWidgetItem(reports, { tr("Cast report") });
    castReport->setData(0, Qt::UserRole, StatisticsParameters::Report);
    castReport->setData(0, Qt::UserRole + 1, StatisticsParameters::CastReport);
    castReport->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/file-document-box.svg"));
    QTreeWidgetItem* charactersDialoguesReport = new QTreeWidgetItem(reports, { tr("Characters dialogues") });
    charactersDialoguesReport->setData(0, Qt::UserRole, StatisticsParameters::Report);
    charactersDialoguesReport->setData(0, Qt::UserRole + 1, StatisticsParameters::CharacterReport);
    charactersDialoguesReport->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/file-document-box.svg"));
    m_statisticTypes->addTopLevelItem(reports);

    //
    // Настраиваем панель со списком графиков
    //
    QTreeWidgetItem* plots = new QTreeWidgetItem(m_statisticTypes, { tr("Plots") });
    plots->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/clipboard-pulse.svg"));
    QTreeWidgetItem* storyStructureAnalisysPlot = new QTreeWidgetItem(plots, { tr("Story structure analysis") });
    storyStructureAnalisysPlot->setData(0, Qt::UserRole, StatisticsParameters::Plot);
    storyStructureAnalisysPlot->setData(0, Qt::UserRole + 1, StatisticsParameters::StoryStructureAnalisysPlot);
    storyStructureAnalisysPlot->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/chart-line.svg"));
    QTreeWidgetItem* charactersActivityPlot = new QTreeWidgetItem(plots, { tr("Characters activity") });
    charactersActivityPlot->setData(0, Qt::UserRole, StatisticsParameters::Plot);
    charactersActivityPlot->setData(0, Qt::UserRole + 1, StatisticsParameters::CharactersActivityPlot);
    charactersActivityPlot->setData(0, Qt::DecorationRole, QPixmap(":/Graphics/Iconset/chart-line.svg"));
    m_statisticTypes->addTopLevelItem(plots);

    m_statisticTypes->setObjectName("StatisticsTypesTree");
    m_statisticTypes->expandAll();
    m_statisticTypes->setHeaderHidden(true);
    m_statisticTypes->setItemDelegate(new TreeViewItemDelegate(m_statisticTypes));
    m_statisticTypes->setStyle(new TreeViewProxyStyle(m_statisticTypes->style()));

    //
    // Настраиваем общую панель с группами отчётов
    //
    QHBoxLayout* statisticTypesToolbarLayout = new QHBoxLayout;
    statisticTypesToolbarLayout->setContentsMargins(QMargins());
    statisticTypesToolbarLayout->setSpacing(0);
    statisticTypesToolbarLayout->addWidget(m_leftTopEmptyLabel);
    statisticTypesToolbarLayout->addWidget(m_settings);
    //
    QVBoxLayout* statisticTypesLayout = new QVBoxLayout;
    statisticTypesLayout->setContentsMargins(QMargins());
    statisticTypesLayout->setSpacing(0);
    statisticTypesLayout->addLayout(statisticTypesToolbarLayout);
    statisticTypesLayout->addWidget(m_statisticTypes);

    //
    // Настраиваем виджет навигации целиком
    //
    QWidget* statisticTypesPanel = new QWidget(this);
    statisticTypesPanel->setObjectName("statisticTypesPanel");
    statisticTypesPanel->setLayout(statisticTypesLayout);
    m_navigation->addWidget(statisticTypesPanel);
    m_navigation->addWidget(m_statisticSettings);

    //
    // Настраиваем панель с данными по отчётам
    //
    m_reportData->setOpenLinks(false);
    m_reportData->setOpenExternalLinks(false);
    m_statisticData->addWidget(m_reportData);
    m_statisticData->addWidget(m_plotData);

    QHBoxLayout* toolbarLayout = new QHBoxLayout;
    toolbarLayout->setContentsMargins(QMargins());
    toolbarLayout->setSpacing(0);
    toolbarLayout->addWidget(m_update);
    toolbarLayout->addWidget(m_save);
    toolbarLayout->addWidget(m_print);
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
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(m_navigation);
    splitter->addWidget(statisticDataPanel);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(splitter);
    setLayout(layout);
}

void StatisticsView::initPlot()
{
    //
    // Настроим легенду
    //
    m_plotData->legend->setIconSize(50, 20);
    m_plotData->legend->setVisible(true);
    //
    // Настроим оси координат
    //
    m_plotData->xAxis->setLabel(tr("Duration, minutes"));
    m_plotData->xAxis2->setVisible(true);
    m_plotData->xAxis2->setTickLabels(false);
    m_plotData->yAxis2->setVisible(true);
    m_plotData->yAxis2->setTickLabels(false);
    //
    // Настроим возможности взаимодействия с графиком
    //
    m_plotData->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_plotData->axisRect()->setRangeZoom(Qt::Horizontal);
    m_plotData->axisRect()->setRangeDragAxes(m_plotData->xAxis, 0);
    m_plotData->setNoAntialiasingOnDrag(true);
    //
    // Настроим цвета в соответствии с палитрой
    //
    m_plotData->xAxis->setBasePen(QPen(palette().text(), 1));
    m_plotData->yAxis->setBasePen(QPen(palette().text(), 1));
    m_plotData->xAxis->setTickPen(QPen(palette().text(), 1));
    m_plotData->yAxis->setTickPen(QPen(palette().text(), 1));
    m_plotData->xAxis->setSubTickPen(QPen(palette().text(), 1));
    m_plotData->yAxis->setSubTickPen(QPen(palette().text(), 1));
    m_plotData->xAxis->setTickLabelColor(palette().text().color());
    m_plotData->yAxis->setTickLabelColor(palette().text().color());
    m_plotData->xAxis->grid()->setPen(QPen(palette().text(), 1, Qt::DotLine));
    m_plotData->yAxis->grid()->setPen(QPen(palette().text(), 1, Qt::DotLine));
    m_plotData->xAxis->grid()->setSubGridPen(QPen(palette().dark(), 1, Qt::DotLine));
    m_plotData->yAxis->grid()->setSubGridPen(QPen(palette().dark(), 1, Qt::DotLine));
    m_plotData->xAxis->grid()->setSubGridVisible(true);
    m_plotData->yAxis->grid()->setSubGridVisible(true);
    m_plotData->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    m_plotData->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    m_plotData->xAxis->setLabelColor(palette().text().color());
    m_plotData->yAxis->setLabelColor(palette().text().color());
    m_plotData->xAxis2->setBasePen(QPen(palette().text(), 1));
    m_plotData->yAxis2->setBasePen(QPen(palette().text(), 1));
    m_plotData->xAxis2->setTickPen(QPen(palette().text(), 1));
    m_plotData->yAxis2->setTickPen(QPen(palette().text(), 1));
    m_plotData->xAxis2->setSubTickPen(QPen(palette().text(), 1));
    m_plotData->yAxis2->setSubTickPen(QPen(palette().text(), 1));
    m_plotData->legend->setTextColor(palette().text().color());
    m_plotData->legend->setBrush(palette().base());
    m_plotData->legend->setBorderPen(QPen(palette().text(), 1));
    m_plotData->setBackground(palette().base());
    m_plotData->axisRect()->setBackground(palette().base());
}

void StatisticsView::initConnections()
{
    connect(m_statisticTypes, &QTreeWidget::currentItemChanged, this, &StatisticsView::activateReport);
    connect(m_statisticTypes, &QTreeWidget::itemClicked, this, &StatisticsView::activateReport);

    connect(m_settings, &FlatButton::clicked, [this] {
        const int delay = WAF::StackedWidgetAnimation::slide(m_navigation, m_statisticSettings, WAF::FromRightToLeft);
        QTimer::singleShot(delay, this, &StatisticsView::makeReport);
    });

    connect(m_statisticSettings, &StatisticsSettings::backPressed, [this] {
        WAF::StackedWidgetAnimation::slide(m_navigation, m_navigation->widget(0), WAF::FromLeftToRight);
    });
    connect(m_statisticSettings, &StatisticsSettings::settingsChanged, this, &StatisticsView::makeReport);

    connect(m_print, &FlatButton::clicked, this, &StatisticsView::printReport);
    connect(m_save, &FlatButton::clicked, this, &StatisticsView::saveReport);
    connect(m_update, &FlatButton::clicked, [=] {
        //
        // Запоминаем последнюю позицию в отчёте
        //
        const int lastVerticalScrollValue = m_reportData->verticalScrollBar()->value();
        //
        // Обновляем отчёт
        //
        makeReport();
        //
        // Восстанавливаем позицию
        //
        m_reportData->verticalScrollBar()->setValue(lastVerticalScrollValue);
    });

    connect(m_reportData, &QTextBrowser::anchorClicked, this, &StatisticsView::linkActivated);
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
    m_reportData->setProperty("nobordersContainer", true);
}

