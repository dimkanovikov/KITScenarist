#include "ScenarioNavigator.h"

#include "ScenarioNavigatorItemDelegate.h"

#include <QLabel>
#include <QTreeView>
#include <QHBoxLayout>
#include <QVBoxLayout>

using UserInterface::ScenarioNavigator;
using UserInterface::ScenarioNavigatorItemDelegate;


ScenarioNavigator::ScenarioNavigator(QWidget *parent) :
	QWidget(parent),
	m_scenesCount(new QLabel(this)),
	m_navigationTree(new QTreeView(this)),
	m_navigationTreeDelegate(new ScenarioNavigatorItemDelegate(this))
{
	initView();
	initConnections();
	initStyleSheet();
}

void ScenarioNavigator::setScenesCount(int _scenesCount)
{
	m_scenesCount->setText(QString::number(_scenesCount));
}

void ScenarioNavigator::setModel(QAbstractItemModel* _model)
{
	m_navigationTree->setModel(_model);
}

void ScenarioNavigator::setCurrentIndex(const QModelIndex& _index)
{
	m_navigationTree->setCurrentIndex(_index);
}

void ScenarioNavigator::setShowSceneNumber(bool _show)
{
	m_navigationTreeDelegate->setShowSceneNumber(_show);
}

void ScenarioNavigator::initView()
{
	m_title = new QLabel(tr("Navigator"), this);
	m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_scenesCountTitle = new QLabel(tr("Scenes: "), this);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_title);
	topLayout->addWidget(m_scenesCountTitle);
	topLayout->addWidget(m_scenesCount);

	m_navigationTree->setItemDelegate(m_navigationTreeDelegate);
    m_navigationTree->setDragDropMode(QAbstractItemView::DragDrop);
    m_navigationTree->setDragEnabled(true);
    m_navigationTree->setDropIndicatorShown(true);
	m_navigationTree->setAlternatingRowColors(true);
	m_navigationTree->setHeaderHidden(true);
	m_navigationTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(topLayout);
	layout->addWidget(m_navigationTree);

	setLayout(layout);
}

void ScenarioNavigator::initConnections()
{
	connect(m_navigationTree, SIGNAL(activated(QModelIndex)), this, SIGNAL(sceneChoosed(QModelIndex)));
}

void ScenarioNavigator::initStyleSheet()
{
	m_title->setProperty("inTopPanel", true);
	m_title->setProperty("topPanelTopBordered", true);

	m_scenesCountTitle->setProperty("inTopPanel", true);
	m_scenesCountTitle->setProperty("topPanelTopBordered", true);

	m_scenesCount->setProperty("inTopPanel", true);
	m_scenesCount->setProperty("topPanelTopBordered", true);
	m_scenesCount->setProperty("topPanelRightBordered", true);

	m_navigationTree->setProperty("mainContainer", true);
}
