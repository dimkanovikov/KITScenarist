#include "StatisticsManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

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
		delete m_exportedScenario;
		m_exportedScenario = 0;

		m_exportedScenario = _scenario;

		m_needUpdateScenario = false;
	}
}

void StatisticsManager::aboutMakeReport(const BusinessLogic::ReportParameters& _parameters)
{
	//
	// Уведомляем пользователя, о том, что началась генерация отчёта
	//

	//
	// Запрашиваем экспортируемый документ, если нужно
	//
	if (m_needUpdateScenario) {
		emit needNewExportedScenario();
	}

	//
	// Формируем отчёт
	//

	//
	// Устанавливаем отчёт в форму
	//
	m_view->setReport(m_exportedScenario->toHtml());

	//
	// Закрываем уведомление
	//
}

void StatisticsManager::initView()
{
	//
	// Загрузить модель элементов сценария
	//
	QStringList elements;
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::SceneHeading, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::SceneCharacters, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Action, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Character, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Parenthetical, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Dialogue, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Transition, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Note, BEAUTIFY_BLOCK_NAME);
	elements << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Title, BEAUTIFY_BLOCK_NAME);
	m_view->setScriptElements(new QStringListModel(elements, this));
}

void StatisticsManager::initConnections()
{
	connect(m_view, SIGNAL(makeReport(BusinessLogic::ReportParameters)), this, SLOT(aboutMakeReport(BusinessLogic::ReportParameters)));
}

