#include "ScenarioNavigator.h"

#include "ScenarioNavigatorItemDelegate.h"
#include "ScenarioNavigatorProxyStyle.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QAction>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidgetAction>

using UserInterface::ScenarioNavigator;
using UserInterface::ScenarioNavigatorItemDelegate;
using UserInterface::ScenarioNavigatorProxyStyle;


ScenarioNavigator::ScenarioNavigator(QWidget *parent) :
	QWidget(parent),
	m_draftTitle(new QLabel(this)),
	m_scenesCountTitle(new QLabel(this)),
	m_scenesCount(new QLabel(this)),
	m_addItem(new FlatButton(this)),
	m_removeItem(new FlatButton(this)),
	m_middleTitle(new QLabel(this)),
	m_showDraft(new FlatButton(this)),
	m_showNote(new FlatButton(this)),
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
	if (!m_navigationTree->selectionModel()->selectedIndexes().contains(_index)) {
		m_navigationTree->clearSelection();
		m_navigationTree->setCurrentIndex(_index);
	}
}

void ScenarioNavigator::setShowSceneNumber(bool _show)
{
	m_navigationTreeDelegate->setShowSceneNumber(_show);
}

void ScenarioNavigator::setShowSceneDescription(bool _show)
{
	m_navigationTreeDelegate->setShowSceneDescription(_show);
}

void ScenarioNavigator::setSceneDescriptionIsSceneText(bool _isSceneText)
{
	m_navigationTreeDelegate->setSceneDescriptionIsSceneText(_isSceneText);
}

void ScenarioNavigator::setSceneDescriptionHeight(int _height)
{
	m_navigationTreeDelegate->setSceneDescriptionHeight(_height);
}

void ScenarioNavigator::resetView()
{
	m_navigationTree->setItemDelegate(0);
	delete m_navigationTreeDelegate;
	m_navigationTreeDelegate = 0;

	m_navigationTreeDelegate = new ScenarioNavigatorItemDelegate(this);
	m_navigationTree->setItemDelegate(m_navigationTreeDelegate);
}

void ScenarioNavigator::setIsDraft(bool _isDraft)
{
	m_draftTitle->setVisible(_isDraft);
	m_scenesCountTitle->setVisible(!_isDraft);
	m_scenesCount->setVisible(!_isDraft);
	m_middleTitle->setVisible(!_isDraft);
	m_showDraft->setVisible(!_isDraft);
	m_showNote->setVisible(!_isDraft);
}

void ScenarioNavigator::clearSelection()
{
	m_navigationTree->selectionModel()->clearSelection();
}

void ScenarioNavigator::setDraftVisible(bool _visible)
{
	m_showDraft->setChecked(_visible);
	m_showDraft->repaint();
}

void ScenarioNavigator::setNoteVisible(bool _visible)
{
	m_showNote->setChecked(_visible);
	m_showNote->repaint();
}

void ScenarioNavigator::setCommentOnly(bool _isCommentOnly)
{
	m_addItem->setVisible(!_isCommentOnly);
	m_removeItem->setVisible(!_isCommentOnly);
	m_navigationTree->setDragDropMode(_isCommentOnly ? QAbstractItemView::NoDragDrop : QAbstractItemView::DragDrop);
	m_navigationTree->setSelectionMode(_isCommentOnly ? QAbstractItemView::SingleSelection : QAbstractItemView::ContiguousSelection);
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

void ScenarioNavigator::aboutContextMenuRequested(const QPoint& _pos)
{
	QMenu* menu = new QMenu(this);
	//
	// Цвет
	//
	QAction* color = menu->addAction(tr("Color"));
	QMenu* colorMenu = new QMenu(this);
	QWidgetAction* wa = new QWidgetAction(colorMenu);
	GoogleColorsPane* colorsPane = new GoogleColorsPane(colorMenu);
	colorsPane->setCurrentColor(
				m_navigationTree->currentIndex().data(
					BusinessLogic::ScenarioModel::ColorIndex).value<QColor>()
				);
	wa->setDefaultWidget(colorsPane);
	QAction* removeColor = colorMenu->addAction(tr("Clear"));
	colorMenu->addAction(wa);
	color->setMenu(colorMenu);

	connect(colorsPane, SIGNAL(selected(QColor)), menu, SLOT(close()));
	//
	// Остальное
	//
	menu->addSeparator();
	QAction* addNew = menu->addAction(tr("Create After"));
	QAction* remove = menu->addAction(tr("Remove"));

	QAction* toggled = menu->exec(mapToGlobal(_pos));
	if (toggled != 0) {
		if (toggled == removeColor) {
			emit setItemColor(m_navigationTree->currentIndex(), QColor());
		} else if (toggled == addNew) {
			aboutAddItem();
		} else if (toggled == remove) {
			aboutRemoveItem();
		}
	} else {
		if (colorsPane->currentColor().isValid()) {
			emit setItemColor(m_navigationTree->currentIndex(), colorsPane->currentColor());
		}
	}

	menu->deleteLater();
}

void ScenarioNavigator::initView()
{
	setContextMenuPolicy(Qt::CustomContextMenu);

	m_draftTitle->setText(tr("Draft:"));

	m_scenesCountTitle->setText(tr("Scenes:"));
	m_scenesCount->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_scenesCount->setToolTip(tr("Scenes Count"));

	m_addItem->setIcons(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_addItem->setToolTip(tr("Add Scenario Item After Selected"));

	m_removeItem->setIcons(QIcon(":/Graphics/Icons/Editing/delete.png"));
	m_removeItem->setToolTip(tr("Remove Selected Scenario Item"));

	m_middleTitle->setFixedWidth(1);

	m_showDraft->setIcons(QIcon(":/Graphics/Icons/Editing/draft.png"));
	m_showDraft->setToolTip(tr("Show/hide draft"));
	m_showDraft->setCheckable(true);

	m_showNote->setIcons(QIcon(":/Graphics/Icons/Editing/note.png"));
	m_showNote->setToolTip(tr("Show/hide scene note"));
	m_showNote->setCheckable(true);

	m_endTitle->setFixedWidth(1);

	m_navigationTree->setItemDelegate(m_navigationTreeDelegate);
	m_navigationTree->setDragDropMode(QAbstractItemView::DragDrop);
	m_navigationTree->setDragEnabled(true);
	m_navigationTree->setDropIndicatorShown(true);
	m_navigationTree->setAlternatingRowColors(true);
	m_navigationTree->setHeaderHidden(true);
	m_navigationTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_navigationTree->setSelectionMode(QAbstractItemView::ContiguousSelection);
	m_navigationTree->setStyle(new ScenarioNavigatorProxyStyle(m_navigationTree->style()));
	m_navigationTree->installEventFilter(this);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_draftTitle);
	topLayout->addWidget(m_scenesCountTitle);
	topLayout->addWidget(m_scenesCount);
	topLayout->addWidget(m_addItem);
	topLayout->addWidget(m_removeItem);
	topLayout->addWidget(m_middleTitle);
	topLayout->addWidget(m_showDraft);
	topLayout->addWidget(m_showNote);
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
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(aboutContextMenuRequested(QPoint)));

	connect(m_addItem, SIGNAL(clicked()), this, SLOT(aboutAddItem()));
	connect(m_removeItem, SIGNAL(clicked()), this, SLOT(aboutRemoveItem()));
	connect(m_showDraft, SIGNAL(clicked()), this, SIGNAL(showHideDraft()));
	connect(m_showNote, SIGNAL(clicked()), this, SIGNAL(showHideNote()));
	connect(m_navigationTree, SIGNAL(clicked(QModelIndex)), this, SIGNAL(sceneChoosed(QModelIndex)));
}

void ScenarioNavigator::initStyleSheet()
{
	m_draftTitle->setProperty("inTopPanel", true);
	m_draftTitle->setProperty("topPanelTopBordered", true);

	m_scenesCountTitle->setProperty("inTopPanel", true);
	m_scenesCountTitle->setProperty("topPanelTopBordered", true);

	m_scenesCount->setProperty("inTopPanel", true);
	m_scenesCount->setProperty("topPanelTopBordered", true);

	m_addItem->setProperty("inTopPanel", true);
	m_removeItem->setProperty("inTopPanel", true);

	m_middleTitle->setProperty("inTopPanel", true);
	m_middleTitle->setProperty("topPanelTopBordered", true);
	m_middleTitle->setProperty("topPanelRightBordered", true);

	m_showDraft->setProperty("inTopPanel", true);
	m_showNote->setProperty("inTopPanel", true);

	m_endTitle->setProperty("inTopPanel", true);
	m_endTitle->setProperty("topPanelTopBordered", true);

	m_navigationTree->setProperty("mainContainer", true);
}
