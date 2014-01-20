#include "ScenarioNavigator.h"

#include "NavigatorItemsModel.h"
#include "NavigatorItemDelegate.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>

#include <QTreeView>
#include <QHBoxLayout>


ScenarioNavigator::ScenarioNavigator(QWidget* _parent, ScenarioTextEdit* _editor) :
	QWidget(_parent)
{
	initView(_editor);
	initConnections(_editor);
}

int ScenarioNavigator::scenesCount() const
{
	m_model->scenesCount();
}

void ScenarioNavigator::aboutSelectItemForCurrentScene()
{
	m_view->setCurrentIndex(m_model->indexOfItemUnderCursor());
}

void ScenarioNavigator::aboutStoreTreeViewState()
{
	m_viewState = QTreeViewState::storeState(m_view);
}

void ScenarioNavigator::aboutRestoreTreeViewState()
{
	QTreeViewState::restoreState(m_view, m_viewState);
}

void ScenarioNavigator::initView(ScenarioTextEdit* _editor)
{
	setMinimumWidth(240);

	m_model = new NavigatorItemsModel(this, _editor);

	//
	// Настриваем представление
	//
	m_view = new QTreeView(this);
	// ... настраиваем внешний вид
	m_view->setHeaderHidden(true);
	m_view->setAlternatingRowColors(true);
	m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_view->setItemDelegate(new NavigatorItemDelegate(m_view));
	// ... настраиваем перетаскивание элементов
	m_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
	m_view->setDragEnabled(true);
	m_view->setAcceptDrops(true);
	m_view->setDropIndicatorShown(true);
	// ... устанавливаем модель
	m_view->setModel(m_model);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(m_view);
	layout->setContentsMargins(QMargins());

	this->setLayout(layout);
}

void ScenarioNavigator::initConnections(ScenarioTextEdit* _editor)
{
	connect(m_model, SIGNAL(beginUpdateStructure()), this, SLOT(aboutStoreTreeViewState()));
	connect(m_model, SIGNAL(endUpdateStructure()), this, SLOT(aboutRestoreTreeViewState()));

	connect(m_view, SIGNAL(doubleClicked(QModelIndex)), m_model, SLOT(aboutscrollEditorToItem(QModelIndex)));

	connect(_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutSelectItemForCurrentScene()));
	connect(m_model, SIGNAL(endUpdateStructure()), this, SLOT(aboutSelectItemForCurrentScene()));

	connect(m_model, SIGNAL(endUpdateStructure()), this, SIGNAL(structureChanged()));
}
