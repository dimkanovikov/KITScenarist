#include "ScenarioCardsView.h"
#include "CardsResizer.h"

#include <3rd_party/Helpers/ShortcutHelper.h>

#include <3rd_party/Widgets/ActivityEdit/gui/activityedit.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QVariant>
#include <QWidgetAction>

using UserInterface::ScenarioCardsView;
using UserInterface::CardsResizer;


ScenarioCardsView::ScenarioCardsView(QWidget* _parent) :
	QWidget(_parent),
	m_cardsEdit(new ActivityEdit(_parent)),
	m_addCard(new FlatButton(_parent)),
	m_addNote(new FlatButton(_parent)),
	m_addHLine(new FlatButton(_parent)),
	m_addVLine(new FlatButton(_parent)),
	m_sort(new FlatButton(_parent)),
	m_resizer(new CardsResizer(m_sort)),
	m_moveToDraft(new FlatButton(_parent)),
	m_moveToScript(new FlatButton(_parent)),
	m_toolbarSpacer(new QLabel(_parent))
{
	initView();
	initConnections();
	initStyleSheet();
}

void ScenarioCardsView::clear()
{
	m_cardsEdit->clear();
}

void ScenarioCardsView::load(const QString& _xml)
{
	m_cardsEdit->load(_xml);
}

QString ScenarioCardsView::save() const
{
	return m_cardsEdit->save();
}

void ScenarioCardsView::addCard(const QString& _uuid, int _cardType, const QString& _title,
	const QString& _description, const QString& _colors, bool _isCardFirstInParent)
{
	m_cardsEdit->addCard(_uuid, _cardType, _title, _description, _colors, _isCardFirstInParent);
}

void ScenarioCardsView::updateCard(const QString& _uuid, int _type, const QString& _title,
	const QString& _description, const QString& _colors)
{
	m_cardsEdit->updateCard(_uuid, _type, _title, _description, _colors);
}

void ScenarioCardsView::removeCard(const QString& _uuid)
{
	m_cardsEdit->selectCard(_uuid);
	m_cardsEdit->deleteSelectedItems();
}

void ScenarioCardsView::selectCard(const QString& _uuid)
{
	m_cardsEdit->selectCard(_uuid);
}

QString ScenarioCardsView::selectedCardUuid() const
{
	return m_cardsEdit->selectedCardUuid();
}

void ScenarioCardsView::addNote(const QString& _text)
{
	m_cardsEdit->addNote(_text);
}

void ScenarioCardsView::editNote(const QString& _text)
{
	m_cardsEdit->updateNote(_text);
}

void ScenarioCardsView::addFlowText(const QString& _text)
{
	m_cardsEdit->setFlowText(_text);
}

void ScenarioCardsView::editFlowText(const QString& _text)
{
	m_cardsEdit->setFlowText(_text);
}

void ScenarioCardsView::setCommentOnly(bool _isCommentOnly)
{
	m_addCard->setEnabled(!_isCommentOnly);
	m_addNote->setEnabled(!_isCommentOnly);
	m_addHLine->setEnabled(!_isCommentOnly);
	m_addVLine->setEnabled(!_isCommentOnly);
	m_sort->setEnabled(!_isCommentOnly);
}

void ScenarioCardsView::resortCards()
{
	m_cardsEdit->arrangeCards(m_resizer->cardSize(), m_resizer->cardRatio(), m_resizer->distance(),
                              m_resizer->cardsInLine(), m_resizer->cardsInRow());
}

void ScenarioCardsView::showContextMenu(const QPoint& _pos)
{

}

void ScenarioCardsView::initView()
{
	m_addCard->setIcons(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_addCard->setToolTip(tr("Add new card"));

	m_addNote->setIcons(QIcon(":/Graphics/Icons/Review/comment.png"));
	m_addNote->setToolTip(tr("Add note"));

	m_addHLine->setIcons(QIcon(":/Graphics/Icons/Cards/hline.png"));
	m_addHLine->setToolTip(tr("Add horizontal line"));

	m_addVLine->setIcons(QIcon(":/Graphics/Icons/Cards/vline.png"));
	m_addVLine->setToolTip(tr("Add vertical line"));

	m_sort->setIcons(QIcon(":/Graphics/Icons/Cards/table.png"));
	m_sort->setToolTip(tr("Sort cards"));
	//
	// Настроим меню кнопки упорядочивания карточек по сетке
	//
	m_sort->setPopupMode(QToolButton::MenuButtonPopup);
	{
		QMenu* menu = new QMenu(m_sort);
		QWidgetAction* wa = new QWidgetAction(menu);
		wa->setDefaultWidget(m_resizer);
		menu->addAction(wa);
		m_sort->setMenu(menu);
	}

	m_moveToDraft->setIcons(QIcon(":/Graphics/Icons/Editing/draft.png"));
	m_moveToDraft->setToolTip(tr("Draft"));
	m_moveToDraft->hide();

	m_moveToScript->setIcons(QIcon(":/Graphics/Icons/Mobile/script.png"));
	m_moveToScript->setToolTip(tr("Script"));
	m_moveToScript->hide();

	m_toolbarSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QWidget* toolbar = new QWidget(this);
	QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbar);
	toolbarLayout->setContentsMargins(QMargins());
	toolbarLayout->setSpacing(0);
	toolbarLayout->addWidget(m_addCard);
	toolbarLayout->addWidget(m_addNote);
	toolbarLayout->addWidget(m_addHLine);
	toolbarLayout->addWidget(m_addVLine);
	toolbarLayout->addWidget(m_sort);
	toolbarLayout->addWidget(m_toolbarSpacer);
	toolbarLayout->addWidget(m_moveToDraft);
	toolbarLayout->addWidget(m_moveToScript);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(toolbar);
	layout->addWidget(m_cardsEdit);

	setLayout(layout);
}

void ScenarioCardsView::initConnections()
{
	connect(m_cardsEdit, &ActivityEdit::schemeChanged, this, &ScenarioCardsView::schemeChanged);

	connect(m_addCard, &FlatButton::clicked, this, &ScenarioCardsView::addCardClicked);
    connect(m_cardsEdit, &ActivityEdit::addCardRequest, this, &ScenarioCardsView::addCardClicked);
	connect(m_cardsEdit, &ActivityEdit::editCardRequest, this, &ScenarioCardsView::editCardRequest);
	connect(m_cardsEdit, &ActivityEdit::removeCardRequest, this, &ScenarioCardsView::removeCardRequest);
	connect(m_cardsEdit, &ActivityEdit::cardMoved, this, &ScenarioCardsView::cardMoved);
    connect(m_cardsEdit, &ActivityEdit::cardColorsChanged, this, &ScenarioCardsView::cardColorsChanged);

	connect(m_addNote, &FlatButton::clicked, this, &ScenarioCardsView::addNoteClicked);
	connect(m_cardsEdit, &ActivityEdit::editNoteRequest, this, &ScenarioCardsView::editNoteRequest);

	connect(m_cardsEdit, &ActivityEdit::addFlowTextRequest, this, &ScenarioCardsView::addFlowTextRequest);
	connect(m_cardsEdit, &ActivityEdit::editFlowTextRequest, this, &ScenarioCardsView::editFlowTextRequest);

	connect(m_addHLine, &FlatButton::clicked, m_cardsEdit, &ActivityEdit::addHorizontalLine);
	connect(m_addVLine, &FlatButton::clicked, m_cardsEdit, &ActivityEdit::addVerticalLine);

	connect(m_sort, &FlatButton::clicked, this, &ScenarioCardsView::resortCards);
	connect(m_resizer, &CardsResizer::parametersChanged, this, &ScenarioCardsView::resortCards);
}

void ScenarioCardsView::initStyleSheet()
{
	m_addCard->setProperty("inTopPanel", true);
	m_addNote->setProperty("inTopPanel", true);
	m_addHLine->setProperty("inTopPanel", true);
	m_addVLine->setProperty("inTopPanel", true);
	m_sort->setProperty("inTopPanel", true);
	m_sort->setProperty("hasMenu", true);
	m_moveToDraft->setProperty("inTopPanel", true);
	m_moveToScript->setProperty("inTopPanel", true);

	m_toolbarSpacer->setProperty("inTopPanel", true);
	m_toolbarSpacer->setProperty("topPanelTopBordered", true);

	m_cardsEdit->setProperty("mainContainer", true);
}
