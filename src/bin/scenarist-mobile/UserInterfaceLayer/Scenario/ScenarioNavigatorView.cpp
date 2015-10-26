#include "ScenarioNavigatorView.h"
#include "ui_ScenarioNavigatorView.h"

#include <UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorItemDelegate.h>
#include <UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorProxyStyle.h>

#include <3rd_party/Helpers/ScrollerHelper.h>

using UserInterface::ScenarioNavigatorView;
using UserInterface::ScenarioNavigatorItemDelegate;
using UserInterface::ScenarioNavigatorProxyStyle;


ScenarioNavigatorView::ScenarioNavigatorView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ScenarioNavigatorView),
	m_navigatorDelegate(new ScenarioNavigatorItemDelegate(this))
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

ScenarioNavigatorView::~ScenarioNavigatorView()
{
	delete m_ui;
}

QWidget* ScenarioNavigatorView::toolbar() const
{
	return m_ui->toolbar;
}

void ScenarioNavigatorView::setScenarioName(const QString& _name)
{
	m_ui->scenarioName->setText(_name);
}

//void ScenarioNavigatorView::setScenesCount(int _scenesCount)
//{
//	m_scenesCount->setText(QString::number(_scenesCount));
//}

void ScenarioNavigatorView::setModel(QAbstractItemModel* _model)
{
	m_ui->navigator->setModel(_model);
}

void ScenarioNavigatorView::setCurrentIndex(const QModelIndex& _index)
{
	if (!m_ui->navigator->selectionModel()->selectedIndexes().contains(_index)) {
		m_ui->navigator->clearSelection();
		m_ui->navigator->setCurrentIndex(_index);
	}
}

void ScenarioNavigatorView::setShowSceneNumber(bool _show)
{
	m_navigatorDelegate->setShowSceneNumber(_show);
}

void ScenarioNavigatorView::setShowSceneDescription(bool _show)
{
	m_navigatorDelegate->setShowSceneDescription(_show);
}

void ScenarioNavigatorView::setSceneDescriptionIsSceneText(bool _isSceneText)
{
	m_navigatorDelegate->setSceneDescriptionIsSceneText(_isSceneText);
}

void ScenarioNavigatorView::setSceneDescriptionHeight(int _height)
{
	m_navigatorDelegate->setSceneDescriptionHeight(_height);
}

void ScenarioNavigatorView::resetView()
{
	m_ui->navigator->setItemDelegate(0);
	delete m_navigatorDelegate;
	m_navigatorDelegate = 0;

	m_navigatorDelegate = new ScenarioNavigatorItemDelegate(this);
	m_ui->navigator->setItemDelegate(m_navigatorDelegate);
}

void ScenarioNavigatorView::clearSelection()
{
	m_ui->navigator->selectionModel()->clearSelection();
}

void ScenarioNavigatorView::initView()
{
	m_ui->scenarioName->setElideMode(Qt::ElideRight);

	m_ui->navigator->setItemDelegate(m_navigatorDelegate);
	m_ui->navigator->setDragDropMode(QAbstractItemView::DragDrop);
	m_ui->navigator->setDragEnabled(true);
	m_ui->navigator->setDropIndicatorShown(true);
	m_ui->navigator->setAlternatingRowColors(true);
	m_ui->navigator->setHeaderHidden(true);
	m_ui->navigator->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_ui->navigator->setSelectionMode(QAbstractItemView::ContiguousSelection);
	m_ui->navigator->setStyle(new ScenarioNavigatorProxyStyle(m_ui->navigator->style()));

	ScrollerHelper::addScroller(m_ui->navigator);
}

void ScenarioNavigatorView::initConnections()
{
	connect(m_ui->text, &QToolButton::clicked, this, &ScenarioNavigatorView::showTextClicked);
	connect(m_ui->navigator, &QTreeView::clicked, [=](const QModelIndex& _index){
		emit sceneChoosed(_index);
		emit showTextClicked();
	});
}

void ScenarioNavigatorView::initStyleSheet()
{
	m_ui->toolbar->setProperty("toolbar", true);
	m_ui->scenarioName->setProperty("toolbar", true);
	m_ui->search->hide();
	m_ui->menu->hide();
}
