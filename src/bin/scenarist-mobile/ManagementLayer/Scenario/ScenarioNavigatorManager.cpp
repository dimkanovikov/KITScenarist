#include "ScenarioNavigatorManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioNavigatorView.h>
//#include <UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using ManagementLayer::ScenarioNavigatorManager;
using BusinessLogic::ScenarioModel;
using BusinessLogic::ScenarioModelFiltered;
using UserInterface::ScenarioNavigatorView;
using UserInterface::ScenarioItemDialog;


ScenarioNavigatorManager::ScenarioNavigatorManager(QObject *_parent, QWidget* _parentWidget, bool _isDraft) :
	QObject(_parent),
	m_view(new ScenarioNavigatorView(_parentWidget)),
	m_scenarioModel(0),
	m_scenarioModelProxy(new ScenarioModelFiltered(this)),
//	m_addItemDialog(new ScenarioItemDialog(m_view)),
	m_isDraft(_isDraft)
{
	initView();
	initConnections();
	reloadNavigatorSettings();
}

QWidget* ScenarioNavigatorManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* ScenarioNavigatorManager::view() const
{
	return m_view;
}

void ScenarioNavigatorManager::setScenarioName(const QString& _name)
{
	m_view->setScenarioName(_name);
}

void ScenarioNavigatorManager::setNavigationModel(ScenarioModel* _model)
{
	disconnectModel();
	m_scenarioModel = _model;
	m_scenarioModelProxy->setSourceModel(m_scenarioModel);
	m_view->setModel(m_scenarioModelProxy);
	connectModel();

//	if (m_scenarioModel != 0) {
//		aboutModelUpdated();
//	}
}

void ScenarioNavigatorManager::reloadNavigatorSettings()
{
	//
	// Сбросим представление
	//
	m_view->resetView();
	m_view->setShowSceneNumber(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scenes-numbers",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setShowSceneDescription(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scene-description",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setSceneDescriptionIsSceneText(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/scene-description-is-scene-text",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setSceneDescriptionHeight(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/scene-description-height",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
}

void ScenarioNavigatorManager::setCurrentIndex(const QModelIndex& _index)
{
	m_view->setCurrentIndex(m_scenarioModelProxy->mapFromSource(_index));
}

void ScenarioNavigatorManager::clearSelection()
{
	m_view->clearSelection();
}

void ScenarioNavigatorManager::setCommentOnly(bool _isCommentOnly)
{
//	m_view->setCommentOnly(_isCommentOnly);
}

//void ScenarioNavigatorManager::aboutAddItem(const QModelIndex& _index)
//{
//	m_addItemDialog->clearText();

//	//
//	// Если пользователь действительно хочет добавить элемент
//	//
//	if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
//		const int itemType = m_addItemDialog->itemType();
//		const QString header = m_addItemDialog->header();
//		const QColor color = m_addItemDialog->color();
//		const QString description = m_addItemDialog->description();

//		//
//		// Если задан заголовок
//		//
//		if (!header.isEmpty()) {
//			emit addItem(m_scenarioModelProxy->mapToSource(_index), itemType, header, color, description);
//		}
//	}
//}

void ScenarioNavigatorManager::aboutRemoveItems(const QModelIndexList& _indexes)
{
	QModelIndexList removeIndexes;
	foreach (const QModelIndex& index, _indexes) {
		removeIndexes.append(m_scenarioModelProxy->mapToSource(index));
	}
	emit removeItems(removeIndexes);
}

void ScenarioNavigatorManager::aboutSetItemColors(const QModelIndex& _index, const QString& _colors)
{
	emit setItemColors(m_scenarioModelProxy->mapToSource(_index), _colors);
}

void ScenarioNavigatorManager::aboutSceneChoosed(const QModelIndex& _index)
{
	emit sceneChoosed(m_scenarioModelProxy->mapToSource(_index));
}

//void ScenarioNavigatorManager::aboutModelUpdated()
//{
//	m_view->setScenesCount(m_scenarioModel->scenesCount());
//}

void ScenarioNavigatorManager::initView()
{

}

void ScenarioNavigatorManager::initConnections()
{
	connectModel();

	connect(m_view, &ScenarioNavigatorView::showTextClicked, this, &ScenarioNavigatorManager::showTextRequested);

//	connect(m_view, SIGNAL(addItem(QModelIndex)), this, SLOT(aboutAddItem(QModelIndex)));
//	connect(m_view, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
//	connect(m_view, SIGNAL(setItemColors(QModelIndex,QString)), this, SLOT(aboutSetItemColors(QModelIndex,QString)));
	connect(m_view, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutSceneChoosed(QModelIndex)));
//	connect(m_view, SIGNAL(undoPressed()), this, SIGNAL(undoPressed()));
//	connect(m_view, SIGNAL(redoPressed()), this, SIGNAL(redoPressed()));
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

void ScenarioNavigatorManager::initStyleSheet()
{

}
