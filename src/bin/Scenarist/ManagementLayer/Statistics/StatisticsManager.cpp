#include "StatisticsManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/Statistics/ReportFacade.h>

#include <Domain/Character.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

#include <UserInterfaceLayer/Statistics/StatisticsView.h>

#include <QEventLoop>
#include <QStringListModel>
#include <QTextDocument>

using BusinessLogic::ScenarioBlockStyle;
using ManagementLayer::StatisticsManager;
using UserInterface::StatisticsView;

namespace {
	/**
	 * @brief Флаг для получения красивых названий блоков
	 */
	const bool BEAUTIFY_BLOCK_NAME = true;
}


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
	m_view->setCharacters(DataStorageLayer::StorageFacade::characterStorage()->all());
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

void StatisticsManager::aboutMakeReport(const BusinessLogic::ReportParameters& _parameters)
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

	//
	// Формируем отчёт
	//
	const QString reportHtml = BusinessLogic::ReportFacade::makeReport(m_exportedScenario, _parameters);


	//
	// Устанавливаем отчёт в форму
	//
	m_view->setReport(reportHtml);


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
	connect(m_view, SIGNAL(makeReport(BusinessLogic::ReportParameters)), this, SLOT(aboutMakeReport(BusinessLogic::ReportParameters)));
}

