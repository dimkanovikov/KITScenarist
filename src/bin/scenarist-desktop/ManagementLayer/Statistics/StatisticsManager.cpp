#include "StatisticsManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/Statistics/StatisticsFacade.h>
#include <BusinessLayer/Statistics/Reports/AbstractReport.h>

#include <Domain/Research.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ResearchStorage.h>

#include <UserInterfaceLayer/Statistics/StatisticsView.h>

#include <QEventLoop>
#include <QStringListModel>
#include <QTextDocument>

using BusinessLogic::ScenarioBlockStyle;
using ManagementLayer::StatisticsManager;
using UserInterface::StatisticsView;


StatisticsManager::StatisticsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new StatisticsView(_parentWidget)),
	m_exportedScenario(0),
	m_needUpdateScenario(true)
{
	initView();
	initConnections();
}

QWidget* StatisticsManager::view() const
{
	return m_view;
}

void StatisticsManager::loadCurrentProject()
{
	//
	// Очистим от старых данных
	//
	setExportedScenario(0);
	m_needUpdateScenario = true;
	m_view->setReport(QString::null);

	//
	// Загрузить персонажей
	//
    m_view->setCharacters(DataStorageLayer::StorageFacade::researchStorage()->characters());
}

void StatisticsManager::scenarioTextChanged()
{
	m_needUpdateScenario = true;
}

void StatisticsManager::setExportedScenario(QTextDocument* _scenario)
{
	if (m_exportedScenario != _scenario) {
		m_exportedScenario = _scenario;
		m_needUpdateScenario = false;
	}
}

void StatisticsManager::aboutMakeReport(const BusinessLogic::StatisticsParameters& _parameters)
{
	//
	// Уведомляем пользователя, о том, что началась генерация отчёта
	//
	m_view->showProgress();

	//
	// Запрашиваем экспортируемый документ, если нужно
	//
	if (m_needUpdateScenario) {
		emit needNewExportedScenario();
	}

	switch (_parameters.type) {
		case BusinessLogic::StatisticsParameters::Report: {
			//
			// Формируем отчёт
			//
			const QString reportHtml = BusinessLogic::StatisticsFacade::makeReport(m_exportedScenario, _parameters);

			//
			// Устанавливаем отчёт в форму
			//
			m_view->setReport(reportHtml);

			break;
		}

		case BusinessLogic::StatisticsParameters::Plot: {
			//
			// Формируем данные
			//
			BusinessLogic::Plot plot = BusinessLogic::StatisticsFacade::makePlot(m_exportedScenario, _parameters);

			//
			// Рисуем график по данным
			//
			m_view->setPlot(plot);

			break;
		}
	}


	//
	// Закрываем уведомление
	//
	m_view->hideProgress();
}

void StatisticsManager::initView()
{

}

void StatisticsManager::initConnections()
{
    connect(m_view, &StatisticsView::makeReport, this, &StatisticsManager::aboutMakeReport);
    connect(m_view, &StatisticsView::linkActivated, this, &StatisticsManager::linkActivated);
}

