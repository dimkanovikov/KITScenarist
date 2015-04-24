#include "ScenarioStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ScenarioMapper.h>

#include <Domain/Scenario.h>

#include <QUuid>

using namespace DataStorageLayer;
using namespace DataMappingLayer;

namespace {
	/**
	 * @brief Время длительности сессии
	 */
	const int MAX_HOURS_FOR_SESSION = 2;
}


ScenariosTable* ScenarioStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioMapper()->findAll();
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

Scenario* ScenarioStorage::lastSynced(bool _isDraft)
{
	Scenario* lastSyncedScenario = 0;
	if (all()->count() > 0) {
		foreach (DomainObject* domainObject, all()->toList()) {
			if (Scenario* scenario = dynamic_cast<Scenario*>(domainObject)) {
				if (scenario->isSynced()
					&& scenario->isDraft() == _isDraft) {
					if (lastSyncedScenario) {
						if (lastSyncedScenario->versionStartDatetime() < scenario->versionStartDatetime()) {
							lastSyncedScenario = scenario;
						}
					} else {
						lastSyncedScenario = scenario;
					}
				}
			}
		}
	}
	return lastSyncedScenario;
}

Scenario* ScenarioStorage::findByUuid(const QString& _uuid, bool _isDraft)
{
	Scenario* findedScenario = 0;
	if (all()->count() > 0) {
		foreach (DomainObject* domainObject, all()->toList()) {
			if (Scenario* scenario = dynamic_cast<Scenario*>(domainObject)) {
				if (scenario->uuid() == _uuid
					&& scenario->isDraft() == _isDraft) {
					findedScenario = scenario;
					break;
				}
			}
		}
	}
	return findedScenario;
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
		scenario = new Scenario(Identifier(), _name, _synopsis, _text, currentDateTime,
								currentDateTime, _comment, QUuid::createUuid().toString());
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
		QDateTime nextVersionStartDatetime =
				scenario->versionStartDatetime().addSecs(MAX_HOURS_FOR_SESSION * 60 * 60);
		nextVersionStartDatetime.setUtcOffset(0);
		if (scenario->versionComment() != _comment
			|| nextVersionStartDatetime < currentDateTime) {
			//
			// ... создаём новую версию сценария
			//
			scenario = new Scenario(Identifier(), _name, _synopsis, _text, currentDateTime,
									currentDateTime, _comment, QUuid::createUuid().toString());
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

Scenario* ScenarioStorage::storeActualScenario(const QString& _name, const QString& _additionalInfo,
	const QString& _genre, const QString& _author, const QString& _contacts, const QString& _year,
	const QString& _synopsis, const QString& _text, const bool _isDraft,
	const QDateTime& _versionStartDatetime, const QDateTime& _versionEndDatetime,
	const QString& _versionComment, const QString& _uuid)
{
	//
	// Подготовим параметры для сохранения
	//
	bool needSave = false;
	QDateTime versionStartDatetime;
	QDateTime versionEndDatetime;
	QString versionComment;
	QString uuid;
	//
	// ... сценарии сохраняемые данным способом, всегда приходят с сервера, значит они синхронизированы
	//
	const bool isSynced = true;


	Scenario* scenario = current(_isDraft);

	//
	// Если это просто обновление текущего сценария
	//
	if (scenario != 0
		&& scenario->uuid() == _uuid) {
		//
		// Если это более новая версия
		//
		if (_versionEndDatetime > scenario->versionEndDatetime()) {
			//
			// ... то обновим его
			//
			versionStartDatetime = _versionStartDatetime;
			versionEndDatetime = _versionEndDatetime;
			versionComment = _versionComment;
			uuid = _uuid;
			needSave = true;
		}
	}
	//
	// В противном случае создаём новую версию
	//
	else {
		const QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
		QDateTime nextVersionStartDatetime =
				_versionStartDatetime.addSecs(MAX_HOURS_FOR_SESSION * 60 * 60);
		nextVersionStartDatetime.setUtcOffset(0);
		//
		// Если с момента открытия сессии сценария прошло более, чем MAX_HOURS_FOR_SESSION,
		// тогда создаём версию сценария с новым UUID
		//
		if (nextVersionStartDatetime < currentDateTime) {
			versionStartDatetime = currentDateTime;
			versionEndDatetime = currentDateTime;
			versionComment = QString::null;
			uuid = QUuid::createUuid().toString();
			needSave = true;
		}
		//
		// В противном случае сохраняем в БД версию сценария с заданным UUID и другими значениями
		//
		else {
			versionStartDatetime = _versionStartDatetime;
			versionEndDatetime = _versionEndDatetime;
			versionComment = _versionComment;
			uuid = _uuid;
			needSave = true;
		}
	}

	//
	// Если необходимо, то сохраним с подготовленными параметрами
	//
	if (needSave) {
		scenario =
				storeOldScenario(_name, _additionalInfo, _genre, _author, _contacts,
					_year, _synopsis, _text, _isDraft, versionStartDatetime, versionEndDatetime,
					versionComment, uuid, isSynced);
	}

	return scenario;
}

Scenario* ScenarioStorage::storeOldScenario(const QString& _name, const QString& _additionalInfo,
	const QString& _genre, const QString& _author, const QString& _contacts, const QString& _year,
	const QString& _synopsis, const QString& _text, const bool _isDraft,
	const QDateTime& _versionStartDatetime, const QDateTime& _versionEndDatetime,
	const QString& _versionComment, const QString& _uuid, const bool _isSynced)
{
	Scenario* scenario = findByUuid(_uuid, _isDraft);
	//
	// Если сценарий существует, обновим его
	//
	if (scenario != 0) {
		scenario->setName(_name);
		scenario->setAdditionalInfo(_additionalInfo);
		scenario->setGenre(_genre);
		scenario->setAuthor(_author);
		scenario->setContacts(_contacts);
		scenario->setYear(_year);
		scenario->setSynopsis(_synopsis);
		scenario->setText(_text);
		scenario->setIsDraft(_isDraft);
		scenario->setVersionStartDatetime(_versionStartDatetime);
		scenario->setVersionEndDatetime(_versionEndDatetime);
		scenario->setVersionComment(_versionComment);
		scenario->setIsSynced(_isSynced);
		//
		// ... зафиксируем обновлённый текст в базе данных
		//
		MapperFacade::scenarioMapper()->update(scenario);
	}
	//
	// В противном случае создаём новую версию
	//
	else {
		scenario = new Scenario(Identifier(), _name, _synopsis, _text, _versionStartDatetime,
								_versionEndDatetime, _versionComment, _uuid, _isSynced);
		scenario->setAdditionalInfo(_additionalInfo);
		scenario->setGenre(_genre);
		scenario->setAuthor(_author);
		scenario->setContacts(_contacts);
		scenario->setYear(_year);
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
