#include "ScenarioCardsManager.h"

#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.h>

using ManagementLayer::ScenarioCardsManager;
using UserInterface::ScenarioCardsView;

ScenarioCardsManager::ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioCardsView(_parentWidget)),
    m_scenario(nullptr),
    m_model(nullptr)
{
	initConnections();
}

QWidget* ScenarioCardsManager::view() const
{
    return m_view;
}

QString ScenarioCardsManager::save() const
{
    return m_view->save();
}

void ScenarioCardsManager::load(BusinessLogic::ScenarioModel* _model, const QString& _xml)
{
    //
    // Сохраним модель
    //
    if (m_model != _model) {
        m_model = _model;
    }

    //
    // Загрузим сценарий
    //
    // ... если схема есть, то просто загружаем её
    //
    if (!_xml.isEmpty()) {
        m_view->load(_xml);
    }
    //
    // ... а если схема пуста, сформируем её на основе модели
    //
    else {
        m_view->load(m_model->simpleScheme());
    }
}

void ScenarioCardsManager::clear()
{
    m_view->clear();
    m_model = nullptr;
}

void ScenarioCardsManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioCardsManager::initConnections()
{
	connect(m_view, &ScenarioCardsView::schemeChanged, this, &ScenarioCardsManager::schemeChanged);
}
