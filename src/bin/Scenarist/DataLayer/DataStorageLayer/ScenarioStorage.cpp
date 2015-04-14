#include "ScenarioStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ScenarioMapper.h>

#include <Domain/Scenario.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


ScenariosTable* ScenarioStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioMapper()->findLast();
	}
	return m_all;
}

Scenario* ScenarioStorage::current(bool _isDraft)
{
	Scenario* currentScenario = 0;
	if (all()->count() > 0) {
		foreach (DomainObject* domainObject, all()->toList()) {
			if (Scenario* scenario = dynamic_cast<Scenario*>(domainObject)) {
				if (scenario->isDraft() == _isDraft) {
					if (currentScenario) {
						if (currentScenario->versionStartDatetime() < scenario->versionStartDatetime()) {
							currentScenario = scenario;
						}
					} else {
						currentScenario = scenario;
					}
				}
			}
		}
	}
	return currentScenario;
}

Scenario* ScenarioStorage::storeScenario(const QString& _name, const QString& _synopsis,
	const QString& _text, bool _isDraft, const QString& _comment)
{
	Scenario* scenario = current(_isDraft);
	const QDateTime currentDateTime = QDateTime::currentDateTimeUtc();

	//
	// Если сценарий сохраняется в первый раз
	//
	if (scenario == 0) {
		//
		// ... создаём сценарий
		//
		scenario = new Scenario(Identifier(), _name, _synopsis, _text, currentDateTime, currentDateTime, _comment);
		scenario->setIsDraft(_isDraft);

		//
		// ... сохраним сценарий в базе данных
		//
		MapperFacade::scenarioMapper()->insert(scenario);

		//
		// ... добавим в список
		//
		all()->append(scenario);
	}

	//
	// Если сценарий уже был сохранён
	//
	else {

		//
		// Если сценарий сохраняется с новым комментарием, или
		// Если с момента открытия сессии сценария прошло более, чем MAX_HOURS_FOR_SESSION,
		// тогда создаём новую версию сценария
		//
		const int MAX_HOURS_FOR_SESSION = 2;
		QDateTime nextVersionStartDatetime =
				scenario->versionStartDatetime().addSecs(MAX_HOURS_FOR_SESSION * 60 * 60);
		nextVersionStartDatetime.setUtcOffset(0);
		if (scenario->versionComment() != _comment
			|| nextVersionStartDatetime < currentDateTime) {
			//
			// ... создаём новую версию сценария
			//
			scenario = new Scenario(Identifier(), _name, _synopsis, _text, currentDateTime, currentDateTime, _comment);
			scenario->setIsDraft(_isDraft);

			//
			// ... сохраняем её
			//
			MapperFacade::scenarioMapper()->insert(scenario);

			//
			// ... добавим в список
			//
			all()->append(scenario);
		}
		//
		// В противном случае обновляем последнюю версию
		//
		else {
			//
			// ... обновим сценарий
			//
			scenario->setName(_name);
			scenario->setSynopsis(_synopsis);
			scenario->setText(_text);
			//
			// Если были реальные изменения, то устанавливаем дату их сохранения
			//
			if (!scenario->isChangesStored()) {
				scenario->setVersionEndDatetime(currentDateTime);
			}

			//
			// ... зафиксируем обновлённый текст в базе данных
			//
			MapperFacade::scenarioMapper()->update(scenario);
		}
	}

	return scenario;
}

void ScenarioStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::scenarioMapper()->clear();
}

ScenarioStorage::ScenarioStorage() :
	m_all(0)
{
}
