#include "ScenarioCardsView.h"
#include "CardsResizer.h"

#include <3rd_party/Helpers/ShortcutHelper.h>

#include <3rd_party/Widgets/CardsEdit/CardsView.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QShortcut>
#include <QVariant>
#include <QWidgetAction>

using UserInterface::ScenarioCardsView;
using UserInterface::CardsResizer;


ScenarioCardsView::ScenarioCardsView(bool _isDraft, QWidget* _parent) :
    QWidget(_parent),
    m_cards(new CardsView(_parent)),
    m_active(new FlatButton(_parent)),
    m_addCard(new FlatButton(_parent)),
	m_sort(new FlatButton(_parent)),
	m_resizer(new CardsResizer(m_sort)),
	m_fullscreen(new FlatButton(_parent)),
	m_toolbarSpacer(new QLabel(_parent))
{
    initView(_isDraft);
	initConnections();
	initShortcuts();
	initStyleSheet();
}

void ScenarioCardsView::clear()
{
    m_cards->clear();
}

void ScenarioCardsView::undo()
{
    m_cards->undo();
}

void ScenarioCardsView::redo()
{
    m_cards->redo();
}

void ScenarioCardsView::setUseCorkboardBackground(bool _use)
{
    m_cards->setUseCorkboardBackground(_use);
}

void ScenarioCardsView::setBackgroundColor(const QColor& _color)
{
    m_cards->setBackgroundColor(_color);
}

void ScenarioCardsView::load(const QString& _xml)
{
//	if (m_cards->load(_xml)) {
//		m_cards->saveChanges(true);
//	} else {
//		emit schemeNotLoaded();
//	}
}

QString ScenarioCardsView::save() const
{
    return m_cards->save();
}

void ScenarioCardsView::saveChanges(bool _hasChangesInText)
{
    m_cards->saveChanges(_hasChangesInText);
}

void ScenarioCardsView::addCard(const QString& _uuid, int _cardType, const QString& _title,
	const QString& _description, const QString& _colors, bool _isCardFirstInParent)
{
//	m_cards->addCard(_uuid, _cardType, _title, _description, _colors, _isCardFirstInParent);
}

void ScenarioCardsView::updateCard(const QString& _uuid, int _type, const QString& _title,
	const QString& _description, const QString& _colors)
{
//	m_cards->updateCard(_uuid, _type, _title, _description, _colors);
}

void ScenarioCardsView::removeCard(const QString& _uuid)
{
//	m_cards->selectCard(_uuid);
//	m_cards->deleteSelectedItems();
}

void ScenarioCardsView::selectCard(const QString& _uuid)
{
//	m_cards->selectCard(_uuid);
}

QString ScenarioCardsView::selectedCardUuid() const
{
//	return m_cards->selectedCardUuid();
}
void ScenarioCardsView::setCommentOnly(bool _isCommentOnly)
{
    m_addCard->setEnabled(!_isCommentOnly);
    m_sort->setEnabled(!_isCommentOnly);
}

void ScenarioCardsView::resortCards()
{
//	m_cards->arrangeCards(m_resizer->cardSize(), m_resizer->cardRatio(), m_resizer->distance(),
//							  m_resizer->cardsInLine(), m_resizer->cardsInRow());
}

void ScenarioCardsView::initView(bool _isDraft)
{
    if (_isDraft) {
        m_active->setText(tr("Draft"));
    } else {
        m_cards->setCanAddActs(true);
        m_cards->setFixedMode(true);

        m_active->setText(tr("Script"));
    }
    m_active->setCheckable(true);
    m_active->setToolButtonStyle(Qt::ToolButtonTextOnly);

    m_addCard->setIcons(QIcon(":/Graphics/Icons/Editing/add.png"));
    m_addCard->setToolTip(tr("Add new card"));

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

    m_fullscreen->setIcons(QIcon(":/Graphics/Icons/Editing/fullscreen.png"),
        QIcon(":/Graphics/Icons/Editing/fullscreen_active.png"));
    m_fullscreen->setToolTip(tr("On/off fullscreen mode (F5)"));
    m_fullscreen->setCheckable(true);

    m_toolbarSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget* toolbar = new QWidget(this);
    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(QMargins());
    toolbarLayout->setSpacing(0);
    toolbarLayout->addWidget(m_active);
    toolbarLayout->addWidget(m_addCard);
    toolbarLayout->addWidget(m_sort);
    toolbarLayout->addWidget(m_toolbarSpacer);
    toolbarLayout->addWidget(m_fullscreen);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(toolbar);
    layout->addWidget(m_cards);

    setLayout(layout);
}

void ScenarioCardsView::initConnections()
{
    /*
    connect(ui->addCard, &QPushButton::clicked, [=] {
        ui->draft->addCard(QUuid::createUuid().toString(), false, "Text", "Description", "", "#ffca12");
    });

    connect(ui->draft, &CardsView::cardAddRequest, [=] (const QPointF& _position) {

        ui->draft->addCard(QUuid::createUuid().toString(), false, "Text", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                                                     "Sed pretium, risus eget porta sollicitudin, justo tortor "
                                                     "fermentum massa, ut dictum lacus risus ut dolor. Praesent "
                                                     "sodales ultrices leo. Maecenas pharetra ipsum eu est aliquet,"
                                                     " ac bibendum nisi sollicitudin. In congue rutrum maximus. Nam "
                                                     "pharetra pellentesque quam, vel pulvinar sem ornare eu. Donec "
                                                     "lorem nibh, blandit sit amet vulputate eleifend, dictum a urna. "
                                                     "Aenean ut lorem posuere, auctor ante ac, tincidunt nibh. Sed "
                                                     "dignissim odio sed lectus blandit, eget volutpat purus maximus. "
                                                     "Sed eget odio mollis, rutrum leo a, ornare diam. Maecenas "
                                                     "condimentum tellus eget turpis dictum, vel mattis erat facilisis. "
                                                     "Pellentesque habitant morbi tristique senectus et netus et "
                                                     "malesuada fames ac turpis egestas.",
                              "First draft", "", _position);
    });

    connect(ui->draft, &CardsView::cardAddCopyRequest,
        [=] (bool _isFolder, const QString& _title, const QString& _description, const QString& _state,
             const QString& _colors, const QPointF& _position) {
        ui->draft->addCard(QUuid::createUuid().toString(), _isFolder, _title, _description, _state, _colors, _position);
    });

    connect(ui->saveChanges, &QPushButton::clicked, [=] { ui->draft->saveChanges(); });
    connect(ui->undo, &QPushButton::clicked, ui->draft, &CardsView::undo);
    connect(ui->redo, &QPushButton::clicked, ui->draft, &CardsView::redo);


    // ****
    // Act

    connect(ui->addAct, &QPushButton::clicked, [=] {
        ui->script->addAct(QUuid::createUuid().toString(), "Act", "With some description", "#af51cc");
    });

    connect(ui->script, &CardsView::actAddRequest, [=] {
        ui->script->addAct(QUuid::createUuid().toString(), "Act", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                                                               "Sed pretium, risus eget porta sollicitudin, justo tortor "
                                                               "fermentum massa, ut dictum lacus risus ut dolor. Praesent "
                                                               "sodales ultrices leo. Maecenas pharetra ipsum eu est aliquet,"
                                                               " ac bibendum nisi sollicitudin. In congue rutrum maximus. Nam "
                                                               "pharetra pellentesque quam, vel pulvinar sem ornare eu. Donec "
                                                               "lorem nibh, blandit sit amet vulputate eleifend, dictum a urna. "
                                                               "Aenean ut lorem posuere, auctor ante ac, tincidunt nibh. Sed "
                                                               "dignissim odio sed lectus blandit, eget volutpat purus maximus. "
                                                               "Sed eget odio mollis, rutrum leo a, ornare diam. Maecenas "
                                                               "condimentum tellus eget turpis dictum, vel mattis erat facilisis. "
                                                               "Pellentesque habitant morbi tristique senectus et netus et "
                                                               "malesuada fames ac turpis egestas.", "#cd01a0;#ffac32;#93fac3");
    });

    connect(ui->script, &CardsView::cardAddRequest, [=] (const QPointF& _position) {
        ui->script->addCard(QUuid::createUuid().toString(), true, "Text", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                                                     "Sed pretium, risus eget porta sollicitudin, justo tortor "
                                                     "fermentum massa, ut dictum lacus risus ut dolor. Praesent "
                                                     "sodales ultrices leo. Maecenas pharetra ipsum eu est aliquet,"
                                                     " ac bibendum nisi sollicitudin. In congue rutrum maximus. Nam "
                                                     "pharetra pellentesque quam, vel pulvinar sem ornare eu. Donec "
                                                     "lorem nibh, blandit sit amet vulputate eleifend, dictum a urna. "
                                                     "Aenean ut lorem posuere, auctor ante ac, tincidunt nibh. Sed "
                                                     "dignissim odio sed lectus blandit, eget volutpat purus maximus. "
                                                     "Sed eget odio mollis, rutrum leo a, ornare diam. Maecenas "
                                                     "condimentum tellus eget turpis dictum, vel mattis erat facilisis. "
                                                     "Pellentesque habitant morbi tristique senectus et netus et "
                                                     "malesuada fames ac turpis egestas.",
                              "First draft", "", _position);
    });

    connect(ui->script, &CardsView::cardAddCopyRequest,
        [=] (bool _isFolder, const QString& _title, const QString& _description, const QString& _state,
             const QString& _colors, const QPointF& _position) {
        ui->script->addCard(QUuid::createUuid().toString(), _isFolder, _title, _description, _state, _colors, _position);
    });

    connect(ui->script, &CardsView::actRemoveRequest, ui->script, &CardsView::removeAct);
    connect(ui->script, &CardsView::cardRemoveRequest, ui->script, &CardsView::removeCard);

    // ****

    connect(ui->save, &QPushButton::clicked, [=] {
        QFile draft("c:\\draft.xml");
        draft.open(QIODevice::WriteOnly | QIODevice::Truncate);
        draft.write(ui->draft->save().toUtf8().data());
        draft.close();
        //
        QFile script("c:\\script.xml");
        script.open(QIODevice::WriteOnly | QIODevice::Truncate);
        script.write(ui->script->save().toUtf8().data());
        script.close();
    });

    connect(ui->load, &QPushButton::clicked, [=] {
        QFile draft("c:\\draft.xml");
        draft.open(QIODevice::ReadOnly);
        ui->draft->load(draft.readAll());
        draft.close();
        //
        QFile script("c:\\script.xml");
        script.open(QIODevice::ReadOnly);
        ui->script->load(script.readAll());
        script.close();
    });
     */

//	connect(m_cards, &ActivityEdit::schemeChanged, this, &ScenarioCardsView::schemeChanged);

//	connect(m_addCard, &FlatButton::clicked, this, &ScenarioCardsView::addCardClicked);
//	connect(m_cards, &ActivityEdit::addCardRequest, this, &ScenarioCardsView::addCardClicked);
//	connect(m_cards, &ActivityEdit::editCardRequest, this, &ScenarioCardsView::editCardRequest);
//	connect(m_cards, &ActivityEdit::removeCardRequest, this, &ScenarioCardsView::removeCardRequest);
//	connect(m_cards, &ActivityEdit::cardMoved, this, &ScenarioCardsView::cardMoved);
//	connect(m_cards, &ActivityEdit::cardColorsChanged, this, &ScenarioCardsView::cardColorsChanged);
//	connect(m_cards, &ActivityEdit::itemTypeChanged, this, &ScenarioCardsView::itemTypeChanged);

//	connect(m_addNote, &FlatButton::clicked, this, &ScenarioCardsView::addNoteClicked);
//	connect(m_cards, &ActivityEdit::editNoteRequest, this, &ScenarioCardsView::editNoteRequest);

//	connect(m_cards, &ActivityEdit::addFlowTextRequest, this, &ScenarioCardsView::addFlowTextRequest);
//	connect(m_cards, &ActivityEdit::editFlowTextRequest, this, &ScenarioCardsView::editFlowTextRequest);

//	connect(m_addHLine, &FlatButton::clicked, m_cards, &ActivityEdit::addHorizontalLine);
//	connect(m_addVLine, &FlatButton::clicked, m_cards, &ActivityEdit::addVerticalLine);

//	connect(m_sort, &FlatButton::clicked, this, &ScenarioCardsView::resortCards);
//	connect(m_resizer, &CardsResizer::parametersChanged, this, &ScenarioCardsView::resortCards);

//	connect(m_fullscreen, &FlatButton::clicked, this, &ScenarioCardsView::fullscreenRequest);
}

void ScenarioCardsView::initShortcuts()
{
//	QShortcut* undo = new QShortcut(QKeySequence::Undo, this);
//	undo->setContext(Qt::WidgetWithChildrenShortcut);
//	connect(undo, &QShortcut::activated, [=] {
//		//
//		// Если отмену необходимо синхронизировать с текстом, уведомляем об этом
//		//
//		if (m_cards->needSyncUndo()) {
//			emit undoRequest();
//		}
//		//
//		// А если синхронизировать не нужно, просто отменяем последнее изменение
//		//
//		else {
//			m_cards->saveChanges(false);
//			m_cards->undo();
//		}
//	});

//	QShortcut* redo = new QShortcut(QKeySequence::Redo, this);
//	redo->setContext(Qt::WidgetWithChildrenShortcut);
//	connect(redo, &QShortcut::activated, [=] {
//		//
//		// Если повтор необходимо синхронизировать с текстом, уведомляем об этом
//		//
//		if (m_cards->needSyncRedo()) {
//			emit redoRequest();
//		}
//		//
//		// А если синхронизировать не нужно, просто повторяем последнее изменение
//		//
//		else {
//			m_cards->redo();
//		}
//	});

//	QShortcut* fullscreen = new QShortcut(Qt::Key_F5, this);
//	fullscreen->setContext(Qt::WidgetWithChildrenShortcut);
//	connect(fullscreen, &QShortcut::activated, m_fullscreen, &FlatButton::click);
}

void ScenarioCardsView::initStyleSheet()
{
    m_active->setProperty("inTopPanel", true);
    m_addCard->setProperty("inTopPanel", true);
    m_sort->setProperty("inTopPanel", true);
    m_sort->setProperty("hasMenu", true);
    m_fullscreen->setProperty("inTopPanel", true);

    m_toolbarSpacer->setProperty("inTopPanel", true);
    m_toolbarSpacer->setProperty("topPanelTopBordered", true);
    m_toolbarSpacer->setProperty("topPanelRightBordered", true);

    m_cards->setProperty("mainContainer", true);
}
