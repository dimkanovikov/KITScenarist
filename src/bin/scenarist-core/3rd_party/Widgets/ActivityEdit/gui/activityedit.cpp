#include "activityedit.h"

#include "customgraphicsview.h"
#include "../scene/customgraphicsscene.h"
#include "../scene/sceneundostack.h"
#include "../shape/card.h"
#include "../shape/note.h"
#include "../flow/arrowflow.h"
#include "../xml/load_xml.h"
#include "../xml/save_xml.h"

#include <QVBoxLayout>


ActivityEdit::ActivityEdit(QWidget *parent) :
	QFrame(parent),
	m_view(new CustomGraphicsView(this)),
	m_undoStack(new SceneUndoStack)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_view->setDragMode(QGraphicsView::RubberBandDrag);
	m_view->setRubberBandSelectionMode(Qt::ContainsItemShape);
	m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	CustomGraphicsScene* scene = new CustomGraphicsScene;
	m_view->setScene(scene);

	connect(m_view, &CustomGraphicsView::deletePressed, [=] {
		//
		// При нажатии кнопки удаления, сперва формируем список элементов для удаления
		//
		QStringList uuidsForDelete;
		for (auto shape : scene->selectedShapes()) {
			if (CardShape* card = dynamic_cast<CardShape*>(shape)) {
				uuidsForDelete << card->uuid();
			}
		}
		//
		// ... затем шлём сигналы об удалении сцен
		//
		for (const QString& uuid : uuidsForDelete) {
			emit removeCardRequest(uuid);
		}

		//
		// А потом удаляем все остальные выделенные элементы
		//
        scene->removeSelectedShapes();

        //
        // И уведомляем об изменении сцены
        //
        scene->notifyStateChangeByUser();
	});

	connect(scene, &CustomGraphicsScene::stateChangedByUser, [=] {
		m_undoStack->addState(scene->toXML());
		emit schemeChanged();
	});

    connect(scene, &CustomGraphicsScene::addFlowTextRequest, this, &ActivityEdit::addFlowTextRequest);
	connect(scene, &CustomGraphicsScene::editCardRequest, this, &ActivityEdit::editCardRequest);
	connect(scene, &CustomGraphicsScene::editNoteRequest, this, &ActivityEdit::editNoteRequest);
	connect(scene, &CustomGraphicsScene::editFlowTextRequest, this, &ActivityEdit::editFlowTextRequest);
	connect(scene, &CustomGraphicsScene::cardMoved, this, &ActivityEdit::cardMoved);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_view);
}

ActivityEdit::~ActivityEdit()
{
	delete m_undoStack;
	m_undoStack = nullptr;
}

void ActivityEdit::clear()
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		scene->removeAllShapes();
	}
	m_undoStack->clear();
}

void ActivityEdit::undo()
{
	if (m_undoStack->canUndo()) {
		CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene());
		if (!m_undoStack->canRedo()) {
			m_undoStack->addState(scene->toXML());
			m_undoStack->undo();
		}
		scene->fromXML(m_undoStack->undo(), scene);

		if (m_undoStack->canUndo()) {
			emit schemeChanged();
		}
	}
}

void ActivityEdit::redo()
{
	if (m_undoStack->canRedo()) {
		CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene());
		scene->fromXML(m_undoStack->redo(), scene);

		emit schemeChanged();
	}
}

QString ActivityEdit::save() const
{
	return createSceneXml(m_view->scene(), m_view);
}

void ActivityEdit::load(const QString& _xml)
{
	loadSceneXml(_xml, m_view->scene(), m_view);
}

void ActivityEdit::addCard(const QString& _uuid, int _cardType, const QString& _title,
	const QString& _description, bool _isCardFirstInParent)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendCard(_uuid, (CardShape::CardType)_cardType, _title, _description, _isCardFirstInParent);
        scene->notifyStateChangeByUser();
	}
}

void ActivityEdit::updateCard(const QString& _uuid, int _type, const QString& _title,
	const QString& _description)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		for (auto shape : scene->shapes()) {
			if (CardShape* currentCard = dynamic_cast<CardShape*>(shape)) {
				if (currentCard->uuid() == _uuid) {
					currentCard->setCardType((CardShape::CardType)_type);
					currentCard->setTitle(_title);
					currentCard->setDescription(_description);
                    scene->notifyStateChangeByUser();
					break;
				}
			}
		}
	}
}

void ActivityEdit::addNote(const QString& _text)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendNote(_text);
        scene->notifyStateChangeByUser();
    }
}

void ActivityEdit::updateNote(const QString& _text)
{
    if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
        m_view->setDragMode(QGraphicsView::NoDrag);
        auto selected = scene->selectedShapes();
        if (selected.size() == 1) {
            if (NoteShape* note = dynamic_cast<NoteShape*>(selected.first())) {
                note->setText(_text);
                scene->notifyStateChangeByUser();
            }
        }
    }
}

void ActivityEdit::setFlowText(const QString& _text)
{
    if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
        m_view->setDragMode(QGraphicsView::NoDrag);
        auto selected = scene->selectedShapes();
        if (selected.size() == 1) {
            if (ArrowFlow* note = dynamic_cast<ArrowFlow*>(selected.first())) {
                note->setText(_text);
                scene->notifyStateChangeByUser();
            }
        }
    }
}

void ActivityEdit::addHorizontalLine()
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
        scene->appendHorizontalLine();
        scene->notifyStateChangeByUser();
	}
}

void ActivityEdit::addVerticalLine()
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendVerticalLine();
        scene->notifyStateChangeByUser();
	}
}

void ActivityEdit::selectAll()
{
	QPainterPath path;
	path.addRect(m_view->scene()->sceneRect());
	m_view->scene()->setSelectionArea(path);
	m_view->scene()->setSelectionArea(path);
}

void ActivityEdit::selectCard(const QString& _uuid)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		scene->clearSelection();

		for (auto shape : scene->shapes()) {
			if (CardShape* currentCard = dynamic_cast<CardShape*>(shape)) {
				if (currentCard->uuid() == _uuid) {
					currentCard->setSelected(true);
					break;
				}
			}
		}
	}
}

QString ActivityEdit::selectedCardUuid() const
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		CardShape* selectedCard = nullptr;
		if (!scene->selectedShapes().isEmpty()
			&& scene->selectedShapes().size() == 1
			&& (selectedCard = dynamic_cast<CardShape*>(scene->selectedItems().last()))) {
			return selectedCard->uuid();
		}
	}

	return QString::null;
}

void ActivityEdit::deleteSelectedItems()
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		scene->removeSelectedShapes();
        scene->notifyStateChangeByUser();
	}
}
