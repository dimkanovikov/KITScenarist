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
using UserInterface::ScenarioNavigator;
using UserInterface::ScenarioItemDialog;


ScenarioNavigatorManager::ScenarioNavigatorManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_scenarioModel(0),
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
	m_navigator->setModel(m_scenarioModel);
	connectModel();

	if (m_scenarioModel != 0) {
		aboutModelUpdated();
	}
}

void ScenarioNavigatorManager::reloadNavigatorSettings()
{
	m_navigator->setShowSceneNumber(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scenes-numbers",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
}

void ScenarioNavigatorManager::setCurrentIndex(const QModelIndex& _index)
{
	m_navigator->setCurrentIndex(_index);
}

void ScenarioNavigatorManager::aboutAddItem(const QModelIndex& _index)
{
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
			emit addItem(_index, itemHeader, itemType);
		}
	}
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
	connect(m_navigator, SIGNAL(removeItems(QModelIndexList)), this, SIGNAL(removeItems(QModelIndexList)));
	connect(m_navigator, SIGNAL(sceneChoosed(QModelIndex)), this, SIGNAL(sceneChoosed(QModelIndex)));
	connect(m_navigator, SIGNAL(undoPressed()), this, SIGNAL(undoPressed()));
	connect(m_navigator, SIGNAL(redoPressed()), this, SIGNAL(redoPressed()));
}

void ScenarioNavigatorManager::connectModel()
{
	if (m_scenarioModel != 0) {
		connect(m_scenarioModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(aboutModelUpdated()));
		connect(m_scenarioModel, SIGNAL(mimeDropped(int)), this, SIGNAL(sceneChoosed(int)));
	}
}

void ScenarioNavigatorManager::disconnectModel()
{
	if (m_scenarioModel != 0) {
		disconnect(m_scenarioModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(aboutModelUpdated()));
		disconnect(m_scenarioModel, SIGNAL(mimeDropped(int)), this, SIGNAL(sceneChoosed(int)));
	}
}
