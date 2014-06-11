#include "ScenarioNavigator.h"

#include "ScenarioNavigatorItemDelegate.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

using UserInterface::ScenarioNavigator;
using UserInterface::ScenarioNavigatorItemDelegate;


ScenarioNavigator::ScenarioNavigator(QWidget *parent) :
	QWidget(parent),
	m_scenesCountTitle(new QLabel(this)),
	m_scenesCount(new QLabel(this)),
	m_addItem(new QToolButton(this)),
	m_removeItem(new QToolButton(this)),
	m_endTitle(new QLabel(this)),
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
	m_navigationTree->clearSelection();
	m_navigationTree->setCurrentIndex(_index);
}

void ScenarioNavigator::setShowSceneNumber(bool _show)
{
	m_navigationTreeDelegate->setShowSceneNumber(_show);
}

bool ScenarioNavigator::eventFilter(QObject* _watched, QEvent* _event)
{
    bool isEventFiltered = false;

	if (_watched == m_navigationTree
        && _event->type() == QEvent::KeyPress) {
		//
		// Отлавливаем необходимую комбинацию клавиш
		//
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(_event);
        QString keyCharacter = keyEvent->text();
		if (keyEvent->modifiers().testFlag(Qt::ControlModifier)
            && ((Qt::Key)keyEvent->key() == Qt::Key_Z
                || keyCharacter == "z"
				|| keyCharacter == QString::fromUtf8("я"))) {
			if (keyEvent->modifiers().testFlag(Qt::ShiftModifier)) {
				emit redoPressed();
			} else {
                emit undoPressed();
            }
            isEventFiltered = true;
        }
	}
	//
	// В противном случае выполняется стандартная обработка
	//
	else {
		isEventFiltered = QWidget::eventFilter(_watched, _event);
	}

	return isEventFiltered;
}

void ScenarioNavigator::aboutAddItem()
{
	QModelIndex currentItemIndex = m_navigationTree->currentIndex();

	//
	// Тут не проверяется состояние текущего элемента, если индекс не указывает ни на
	// какой элемент, то новый элемент создаётся в самом конце сценария, а если указывает,
	// то новый элемент создаётся после него
	//
	emit addItem(currentItemIndex);
}

void ScenarioNavigator::aboutRemoveItem()
{
	QModelIndexList indexes = m_navigationTree->selectionModel()->selectedIndexes();
	if (!indexes.isEmpty()) {
		emit removeItems(indexes);
	}
}

void ScenarioNavigator::initView()
{
	m_scenesCountTitle->setText(tr("Scenes:"));
	m_scenesCount->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_scenesCount->setToolTip(tr("Scenes Count"));

	m_addItem->setIcon(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_addItem->setToolTip(tr("Add Scenario Item After Selected"));

	m_removeItem->setIcon(QIcon(":/Graphics/Icons/Editing/delete.png"));
	m_removeItem->setToolTip(tr("Remove Selected Scenario Item"));

	m_endTitle->setFixedWidth(1);

	m_navigationTree->setItemDelegate(m_navigationTreeDelegate);
    m_navigationTree->setDragDropMode(QAbstractItemView::DragDrop);
    m_navigationTree->setDragEnabled(true);
    m_navigationTree->setDropIndicatorShown(true);
	m_navigationTree->setAlternatingRowColors(true);
	m_navigationTree->setHeaderHidden(true);
	m_navigationTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_navigationTree->setSelectionMode(QAbstractItemView::ContiguousSelection);
	m_navigationTree->installEventFilter(this);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_scenesCountTitle);
	topLayout->addWidget(m_scenesCount);
	topLayout->addWidget(m_addItem);
	topLayout->addWidget(m_removeItem);
	topLayout->addWidget(m_endTitle);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(topLayout);
	layout->addWidget(m_navigationTree);

	setLayout(layout);
}

void ScenarioNavigator::initConnections()
{
	connect(m_addItem, SIGNAL(clicked()), this, SLOT(aboutAddItem()));
	connect(m_removeItem, SIGNAL(clicked()), this, SLOT(aboutRemoveItem()));
	connect(m_navigationTree, SIGNAL(activated(QModelIndex)), this, SIGNAL(sceneChoosed(QModelIndex)));
}

void ScenarioNavigator::initStyleSheet()
{
	m_scenesCountTitle->setProperty("inTopPanel", true);
	m_scenesCountTitle->setProperty("topPanelTopBordered", true);

	m_scenesCount->setProperty("inTopPanel", true);
	m_scenesCount->setProperty("topPanelTopBordered", true);

	m_addItem->setProperty("inTopPanel", true);
	m_removeItem->setProperty("inTopPanel", true);

	m_endTitle->setProperty("inTopPanel", true);
	m_endTitle->setProperty("topPanelTopBordered", true);
	m_endTitle->setProperty("topPanelRightBordered", true);

	m_navigationTree->setProperty("mainContainer", true);
}
