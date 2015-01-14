#include "ScenarioNavigatorManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.h>
#include <UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using ManagementLayer::ScenarioNavigatorManager;
using BusinessLogic::ScenarioModel;
using BusinessLogic::ScenarioModelFiltered;
using UserInterface::ScenarioNavigator;
using UserInterface::ScenarioItemDialog;


ScenarioNavigatorManager::ScenarioNavigatorManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_scenarioModel(0),
	m_scenarioModelProxy(new ScenarioModelFiltered(this)),
	m_navigator(new ScenarioNavigator(_parentWidget)),
	m_addItemDialog(new ScenarioItemDialog(m_navigator))
{
	initView();
	initConnections();
	reloadNavigatorSettings();
}

QWidget* ScenarioNavigatorManager::view() const
{
	return m_navigator;
}

void ScenarioNavigatorManager::setNavigationModel(ScenarioModel* _model)
{
	disconnectModel();
	m_scenarioModel = _model;
	m_scenarioModelProxy->setSourceModel(m_scenarioModel);
	m_navigator->setModel(m_scenarioModelProxy);
	connectModel();

	if (m_scenarioModel != 0) {
		aboutModelUpdated();
	}
}

void ScenarioNavigatorManager::reloadNavigatorSettings()
{
	//
	// Сбросим представление
	//
	m_navigator->resetView();
	m_navigator->setShowSceneNumber(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scenes-numbers",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_navigator->setShowSceneDescription(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scene-description",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_navigator->setSceneDescriptionIsSceneText(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/scene-description-is-scene-text",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_navigator->setSceneDescriptionHeight(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/scene-description-height",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
}

void ScenarioNavigatorManager::setCurrentIndex(const QModelIndex& _index)
{
	m_navigator->setCurrentIndex(m_scenarioModelProxy->mapFromSource(_index));
}

void ScenarioNavigatorManager::aboutAddItem(const QModelIndex& _index)
{
	m_addItemDialog->clearText();

	//
	// Если пользователь действительно хочет добавить элемент
	//
	if (m_addItemDialog->exec() == QDialog::Accepted) {
		QString itemHeader = m_addItemDialog->itemHeader();
		int itemType = m_addItemDialog->itemType();

		//
		// Если задан заголовок
		//
		if (!itemHeader.isEmpty()) {
			emit addItem(m_scenarioModelProxy->mapToSource(_index), itemHeader, itemType);
		}
	}
}

void ScenarioNavigatorManager::aboutRemoveItems(const QModelIndexList& _indexes)
{
	QModelIndexList removeIndexes;
	foreach (const QModelIndex& index, _indexes) {
		removeIndexes.append(m_scenarioModelProxy->mapToSource(index));
	}
	emit removeItems(removeIndexes);
}

void ScenarioNavigatorManager::aboutSceneChoosed(const QModelIndex& _index)
{
	emit sceneChoosed(m_scenarioModelProxy->mapToSource(_index));
}

void ScenarioNavigatorManager::aboutModelUpdated()
{
	m_navigator->setScenesCount(m_scenarioModel->scenesCount());
}

void ScenarioNavigatorManager::initView()
{
}

void ScenarioNavigatorManager::initConnections()
{
	connectModel();

	connect(m_navigator, SIGNAL(addItem(QModelIndex)), this, SLOT(aboutAddItem(QModelIndex)));
	connect(m_navigator, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
	connect(m_navigator, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutSceneChoosed(QModelIndex)));
	connect(m_navigator, SIGNAL(undoPressed()), this, SIGNAL(undoPressed()));
	connect(m_navigator, SIGNAL(redoPressed()), this, SIGNAL(redoPressed()));
}

void ScenarioNavigatorManager::connectModel()
{
	if (m_scenarioModel != 0) {
		connect(m_scenarioModelProxy, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(aboutModelUpdated()));
		connect(m_scenarioModel, SIGNAL(mimeDropped(int)), this, SIGNAL(sceneChoosed(int)));
	}
}

void ScenarioNavigatorManager::disconnectModel()
{
	if (m_scenarioModel != 0) {
		disconnect(m_scenarioModelProxy, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(aboutModelUpdated()));
		disconnect(m_scenarioModel, SIGNAL(mimeDropped(int)), this, SIGNAL(sceneChoosed(int)));
	}
}
