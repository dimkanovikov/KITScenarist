#include "ImportManager.h"

#include <Domain/Scenario.h>
#include <Domain/Character.h>
#include <Domain/Location.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Import/DocumentImporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <UserInterfaceLayer/Import/ImportDialog.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QApplication>
#include <QSet>

using ManagementLayer::ImportManager;
using UserInterface::ImportDialog;

namespace {
	/**
	 * @brief Старый вордовский формат не поддерживается
	 */
	const QString MS_DOC_EXTENSION = ".doc";
}


ImportManager::ImportManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_importDialog(new ImportDialog(_parentWidget))
{
	initView();
	initConnections();
}

void ImportManager::importScenario(BusinessLogic::ScenarioDocument* _scenario, int _cursorPosition)
{
	if (m_importDialog->exec() == QLightBoxDialog::Accepted) {
		BusinessLogic::ImportParameters importParameters = m_importDialog->importParameters();

		if (!importParameters.filePath.toLower().endsWith(MS_DOC_EXTENSION)) {
			//
			// Покажем уведомление пользователю
			//
			QLightBoxProgress progress(m_importDialog->parentWidget());
			progress.showProgress(tr("Import"), tr("Please wait. Import can take few minutes."));

			//
			// Получим xml-представление импортируемого сценария
			//
			QString importScenarioXml = BusinessLogic::DocumentImporter().importScenario(importParameters);

			//
			// Загрузим импортируемый текст в сценарий
			//
			// ... определим позицию вставки
			//
			int insertPosition = 0;
			switch (importParameters.insertionMode) {
				case BusinessLogic::ImportParameters::ReplaceDocument: {
					_scenario->clear();
					insertPosition = 0;
					break;
				}

				case BusinessLogic::ImportParameters::ToCursorPosition: {
					insertPosition = _cursorPosition;
					break;
				}

				default:
				case BusinessLogic::ImportParameters::ToDocumentEnd: {
					insertPosition = _scenario->document()->characterCount() - 1;
					break;
				}
			}
			//
			// ... загрузим текст
			//
			_scenario->document()->insertFromMime(insertPosition, importScenarioXml);

			//
			// ... в случае необходимости определяем локации и персонажей
			//
			if (importParameters.findCharactersAndLocations) {
				//
				// Персонажи
				//
				{
					QSet<QString> characters = QSet<QString>::fromList(_scenario->findCharacters());

					//
					// Определить персонажи, которых нет в тексте
					//
					QSet<QString> charactersToDelete;
					foreach (DomainObject* domainObject,
							 DataStorageLayer::StorageFacade::characterStorage()->all()->toList()) {
						Character* character = dynamic_cast<Character*>(domainObject);
						if (!characters.contains(character->name())) {
							charactersToDelete.insert(character->name());
						}
					}

					//
					// Удалить тех, кого нет
					//
					DatabaseLayer::Database::transaction();
					foreach (const QString& character, charactersToDelete) {
						DataStorageLayer::StorageFacade::characterStorage()->removeCharacter(character);
					}
					DatabaseLayer::Database::commit();

					//
					// Добавить новых
					//
					DatabaseLayer::Database::transaction();
					foreach (const QString& character, characters) {
						if (!DataStorageLayer::StorageFacade::characterStorage()->hasCharacter(character)) {
							DataStorageLayer::StorageFacade::characterStorage()->storeCharacter(character);
						}
					}
					DatabaseLayer::Database::commit();
				}

				//
				// Локации
				//
				{
					QSet<QString> locations = QSet<QString>::fromList(_scenario->findLocations());

					//
					// Определить локации, которых нет в тексте
					//
					QSet<QString> locationsToDelete;
					foreach (DomainObject* domainObject,
							 DataStorageLayer::StorageFacade::locationStorage()->all()->toList()) {
						Location* location = dynamic_cast<Location*>(domainObject);
						if (!locations.contains(location->name())) {
							locationsToDelete.insert(location->name());
						}
					}

					//
					// Удалить те, которых нет
					//
					DatabaseLayer::Database::transaction();
					foreach (const QString& location, locationsToDelete) {
						DataStorageLayer::StorageFacade::locationStorage()->removeLocation(location);
					}
					DatabaseLayer::Database::commit();

					//
					// Добавить новых
					//
					DatabaseLayer::Database::transaction();
					foreach (const QString& location, locations) {
						if (!DataStorageLayer::StorageFacade::locationStorage()->hasLocation(location)) {
							DataStorageLayer::StorageFacade::locationStorage()->storeLocation(location);
						}
					}
					DatabaseLayer::Database::commit();
				}
			}

			//
			// Закроем уведомление
			//
			progress.finish();
		}
		//
		// Формат MS DOC не поддерживается, он отображается только для того, чтобы пользователи
		// не теряли свои файлы
		//
		else {
			QLightBoxMessage::information(m_importDialog, tr("File format not supported"),
				tr("Microsoft <b>DOC</b> files are not supported. You need save it to <b>DOCX</b> file and reimport."));
		}
	}
}

void ImportManager::initView()
{

}

void ImportManager::initConnections()
{

}
