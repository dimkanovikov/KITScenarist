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
	m_navigationTree(new QTreeView(this))
{
	initView();
	initConnections();
}

void ScenarioNavigator::setScenesCount(int _scenesCount)
{
	m_scenesCount->setText(QString::number(_scenesCount));
}

void ScenarioNavigator::setModel(QAbstractItemModel* _model)
{
	m_navigationTree->setModel(_model);
}

void ScenarioNavigator::initView()
{
	QLabel* title = new QLabel(tr("Navigator"), this);
	title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QLabel* scenesCountTitle = new QLabel(tr("Scenes: "), this);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(title);
	topLayout->addWidget(scenesCountTitle);
	topLayout->addWidget(m_scenesCount);

	m_navigationTree->setItemDelegate(new ScenarioNavigatorItemDelegate(this));
	m_navigationTree->setDragDropMode(QAbstractItemView::DragDrop);
	m_navigationTree->setDragEnabled(true);
	m_navigationTree->setDropIndicatorShown(true);
	m_navigationTree->setAlternatingRowColors(true);
	m_navigationTree->setHeaderHidden(true);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(topLayout);
	layout->addWidget(m_navigationTree);

	setLayout(layout);
}

void ScenarioNavigator::initConnections()
{

}
