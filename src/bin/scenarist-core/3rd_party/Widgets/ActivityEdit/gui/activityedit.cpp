#include "activityedit.h"

#include "customgraphicsview.h"
#include "../scene/customgraphicsscene.h"
#include "../scene/sceneundostack.h"
#include "../shape/card.h"
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
	connect(scene, &CustomGraphicsScene::stateChangedByUser, [=] {
		m_undoStack->addState(scene->toXML());
		emit schemeChanged();
	});

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

void ActivityEdit::addCard(int _cardType, const QString& _title, const QString& _description)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendCard((CardShape::CardType)_cardType, _title, _description);
	}
}

void ActivityEdit::addNote(const QString& _text)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendNote(_text);
	}
}

void ActivityEdit::addHorizontalLine()
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendHorizontalLine();
	}
}

void ActivityEdit::addVerticalLine()
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendVerticalLine();
	}
}

void ActivityEdit::selectAll()
{
	QPainterPath path;
	path.addRect(m_view->scene()->sceneRect());
	m_view->scene()->setSelectionArea(path);
	m_view->scene()->setSelectionArea(path);
}

void ActivityEdit::deleteSelectedItems()
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		scene->notifyStateChangeByUser();
		scene->removeSelectedShapes();
	}
}
