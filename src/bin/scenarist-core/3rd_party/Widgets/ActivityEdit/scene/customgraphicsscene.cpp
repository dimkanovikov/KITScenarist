#include "customgraphicsscene.h"
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include "../shape/card.h"
#include "../flow/arrowflow.h"
#include "../shape/horizontalline.h"
#include "../shape/verticalline.h"
#include "../shape/note.h"
#include "../xml/save_xml.h"
#include "../xml/load_xml.h"
#include "../flow/flowtext.h"

namespace {
	/**
	 * @brief Расстояния на которые будут сдвигаться фигуры
	 */
	/** @{ */
	const int SHAPE_MICROMOVE_DELTA = 10;
	const int SHAPE_MOVE_DELTA = 100;
	/** @} */

	/**
	 * @brief Расположение карточки относительно связи
	 */
	/** @{ */
	const bool CARD_ON_FLOW_START = true;
	const bool CARD_ON_FLOW_END = false;
	/** @} */
}



CustomGraphicsScene::CustomGraphicsScene(QObject* _parent) :
	QGraphicsScene(_parent),
	m_afterMoving(false)
{
	setItemIndexMethod(QGraphicsScene::NoIndex);

	setBackgroundBrush(Qt::transparent);

	QGraphicsRectItem *item;
	addItem(item = new QGraphicsRectItem(QRectF(0,0,10000,10000)));
	item->setVisible(false);
}

CustomGraphicsScene::~CustomGraphicsScene()
{
	removeAllShapes();
}

void CustomGraphicsScene::appendCard(const QString& _uuid, int _cardType, const QString& _title,
    const QString& _description, bool _isCardFirstInParent)
{
	QPointF scenePosition = sceneRect().center();

	//
	// Если выделена карточка
	//
	CardShape* selectedCard = nullptr;
	CardShape* previousCard = nullptr;
	CardShape* nextCard = nullptr;
	CardShape* parentCard = nullptr;
	if (!selectedItems().isEmpty()
		&& selectedItems().size() == 1
		&& (selectedCard = dynamic_cast<CardShape*>(selectedItems().last()))) {
		//
		// Если карточка вложена в группирующую, то расширяем родителя и вкладываем карту в него
		//
		if (selectedCard->parentItem() != nullptr) {
			//
			// Запомним родителя
			//
			parentCard = dynamic_cast<CardShape*>(selectedCard->parentItem());
		}
		//
		// Если вкладывается первый элемент в группирующую карточку, то она и будет родителем
		//
		if (_isCardFirstInParent) {
			parentCard = selectedCard;
		}

		//
		// Предыдущей будет выделенная
		//
		previousCard = selectedCard;

		//
		// Настроим позицию для добавления новой карточки
		//
		if (_isCardFirstInParent) {
			scenePosition = previousCard->scenePos();
			scenePosition.setX(scenePosition.x() + SHAPE_MOVE_DELTA);
			scenePosition.setY(scenePosition.y() + SHAPE_MOVE_DELTA);
		} else {
			scenePosition = previousCard->scenePos();
			scenePosition.setX(scenePosition.x() + previousCard->boundingRect().width() + SHAPE_MOVE_DELTA);
			scenePosition.setY(scenePosition.y() + previousCard->boundingRect().height() + SHAPE_MOVE_DELTA);
		}

		//
		// Определим карточку, которая будет следовать за новой
		//
		Flow* flow = cardFlow(previousCard, CARD_ON_FLOW_START);
		if (flow != nullptr) {
			nextCard = dynamic_cast<CardShape*>(flow->endShape());
			removeShape(flow);
		}
	}
	//
	// В противном случае добавляем карточку после самой последней карточки, если карточки уже есть
	//
	else if (hasCards() && !_isCardFirstInParent) {
		//
		// Определим последнюю карточку
		//
		Shape* lastCardShape = lastCard();
		previousCard = dynamic_cast<CardShape*>(lastCardShape);

		//
		// Настроим позицию для добавления новой карточки
		//
		scenePosition = previousCard->scenePos();
		scenePosition.setX(scenePosition.x() + previousCard->boundingRect().width() + SHAPE_MOVE_DELTA);
		scenePosition.setY(scenePosition.y() + previousCard->boundingRect().height() + SHAPE_MOVE_DELTA);
	}
	//
	// В противном случае добавляем карточку в самое начало
	//
	else if (hasCards() && _isCardFirstInParent) {
		//
		// Определим первую карточку
		//
		Shape* firstCardShape = firstCard();
		nextCard = dynamic_cast<CardShape*>(firstCardShape);

		//
		// Настроим позицию для добавления новой карточки
		//
		scenePosition = nextCard->scenePos();
		scenePosition.setX(scenePosition.x() - CardShape::DEFAULT_WIDTH - SHAPE_MOVE_DELTA);
		scenePosition.setY(scenePosition.y() - CardShape::DEFAULT_HEIGHT - SHAPE_MOVE_DELTA);
	}
	//
	// В противном случае добавляем карточку по середине видимой части сцены, если подключены представления
	//
	else if (!views().isEmpty()) {
		if (QGraphicsView* view = views().last()) {
			const QRect viewportRect(0, 0, view->viewport()->width(), view->viewport()->height());
			const QRectF visibleSceneRect = view->mapToScene(viewportRect).boundingRect();
			scenePosition = visibleSceneRect.center();
		}
	}

	//
	// Добавляем карточку
	//
    Shape* newCard = createCard(_uuid, _cardType, _title, _description, scenePosition, parentCard);
	insertShape(newCard, previousCard);
	//
	// ... корректируем позицию вкладываемой карточки
	//
	if (parentCard != nullptr) {
		const QPointF newPos = parentCard->mapFromScene(newCard->scenePos());
		const QPointF newBottomRightPos = newPos + QPointF(newCard->boundingRect().width(), newCard->boundingRect().height());
		//
		newCard->setParentItem(parentCard);
		newCard->setPos(newPos);
		//
		// ... и масштабируем родителя, если нужно
		//
		if (!parentCard->contains(newBottomRightPos)) {
			QSizeF newSize = parentCard->size();
			if (newSize.width() <= newBottomRightPos.x()) {
				newSize.setWidth(newBottomRightPos.x() + SHAPE_MICROMOVE_DELTA);
			}
			if (newSize.height() <= newBottomRightPos.y()) {
				newSize.setHeight(newBottomRightPos.y() + SHAPE_MICROMOVE_DELTA);
			}
			parentCard->setSize(newSize);
		}
	}

	//
	// Соединяем с предыдущей
	//
	if (previousCard != nullptr) {
		appendShape(new ArrowFlow(previousCard, newCard, parentCard));
	}

	//
	// Соединяем со следующей
	//
	if (nextCard != nullptr) {
		appendShape(new ArrowFlow(newCard, nextCard, parentCard));
	}
}

void CustomGraphicsScene::appendNote(const QString& _text)
{
	QPointF scenePosition = sceneRect().center();

	//
	// Если выделена карточка, то заметку добавляем справа-сверху на углу карточки
	//
	CardShape* selectedCard = 0;
	if (!selectedItems().isEmpty()
		&& selectedItems().size() == 1
		&& (selectedCard = dynamic_cast<CardShape*>(selectedItems().last()))) {
		scenePosition = selectedCard->scenePos();
		scenePosition.setX(scenePosition.x() + selectedCard->boundingRect().width() - SHAPE_MICROMOVE_DELTA);
		scenePosition.setY(scenePosition.y() - SHAPE_MICROMOVE_DELTA);
	}
	//
	// В противном случае добавляем заметку по середине видимой части сцены, если подключены представления
	//
	else if (!views().isEmpty()) {
		if (QGraphicsView* view = views().last()) {
			const QRect viewportRect(0, 0, view->viewport()->width(), view->viewport()->height());
			const QRectF visibleSceneRect = view->mapToScene(viewportRect).boundingRect();
			scenePosition = visibleSceneRect.center();
		}
	}

	//
	// Добавляем заметку
	//
	appendShape(new NoteShape(_text, scenePosition));
}

void CustomGraphicsScene::appendHorizontalLine()
{
	QPointF scenePosition = sceneRect().center();

	//
	// Если выделена фигура, то новую добавляем чуть ниже
	// Избегаем момента, когда выделена вертикальная линия, чтобы не добавлять линию в самый конец
	//
	Shape* selectedShape = 0;
	const QList<QGraphicsItem*> selectedShapes = selectedItems();
	if (!selectedShapes.isEmpty()
		&& selectedShapes.size() == 1
		&& !dynamic_cast<VerticalLineShape*>(selectedShapes.last())
		&& (selectedShape = dynamic_cast<Shape*>(selectedShapes.last()))) {
		scenePosition = selectedShape->scenePos();
		scenePosition.setY(scenePosition.y() + selectedShape->boundingRect().height()  + SHAPE_MOVE_DELTA);
	}
	//
	// В противном случае добавляем линию по середине видимой части сцены, если подключены представления
	//
	else if (!views().isEmpty()) {
		if (QGraphicsView* view = views().last()) {
			const QRect viewportRect(0, 0, view->viewport()->width(), view->viewport()->height());
			const QRectF visibleSceneRect = view->mapToScene(viewportRect).boundingRect();
			scenePosition = visibleSceneRect.center();
		}
	}

	//
	// Добавляем линию
	//
	appendShape(new HorizontalLineShape(scenePosition));
}

void CustomGraphicsScene::appendVerticalLine()
{
	QPointF scenePosition = sceneRect().center();

	//
	// Если выделена фигура, то новую добавляем чуть правее
	// Избегаем момента, когда выделена горизонтальная линия, чтобы не добавлять линию в самый конец
	//
	Shape* selectedShape = 0;
	const QList<QGraphicsItem*> selectedShapes = selectedItems();
	if (!selectedShapes.isEmpty()
		&& selectedShapes.size() == 1
		&& !dynamic_cast<HorizontalLineShape*>(selectedShapes.last())
		&& (selectedShape = dynamic_cast<Shape*>(selectedShapes.last()))) {
		scenePosition = selectedShape->scenePos();
		scenePosition.setX(scenePosition.x() + selectedShape->boundingRect().width() + SHAPE_MOVE_DELTA);
	}
	//
	// В противном случае добавляем линию по середине видимой части сцены, если подключены представления
	//
	else if (!views().isEmpty()) {
		if (QGraphicsView* view = views().last()) {
			const QRect viewportRect(0, 0, view->viewport()->width(), view->viewport()->height());
			const QRectF visibleSceneRect = view->mapToScene(viewportRect).boundingRect();
			scenePosition = visibleSceneRect.center();
		}
	}

	//
	// Добавляем линию
	//
	appendShape(new VerticalLineShape(scenePosition));
}

QList<Shape*> CustomGraphicsScene::shapes() const
{
	return m_shapes;
}

void CustomGraphicsScene::focusShape(Shape* _shape)
{
	clearSelection();
	_shape->setSelected(true);
	if (!views().isEmpty()) {
		const QRectF itemRect = _shape->mapToScene(_shape->boundingRect()).boundingRect();
		const QRectF visibleRect(
					itemRect.center().x() - SHAPE_MOVE_DELTA,
					itemRect.center().y() - SHAPE_MOVE_DELTA,
					SHAPE_MOVE_DELTA * 2,
					SHAPE_MOVE_DELTA * 2);
		views().last()->ensureVisible(visibleRect);
	}
}

void CustomGraphicsScene::appendShape(Shape* _shape)
{
	//
	// Добавляем фигуру
	//
	connect(_shape, SIGNAL(stateIsAboutToBeChangedByUser()), this, SIGNAL(stateChangedByUser()));
	m_shapes.append(_shape);
	if (!items().contains(_shape)) {
		addItem(_shape);
	}

	//
	// Фокусируем экран на неё, если это не связь
	//
	if (!dynamic_cast<Flow*>(_shape)) {
		focusShape(_shape);
	}
}

void CustomGraphicsScene::insertShape(Shape* _shape, Shape* _after)
{
	//
	// +1 т.к. вставляем после заданного
	//
	connect(_shape, SIGNAL(stateIsAboutToBeChangedByUser()), this, SIGNAL(stateChangedByUser()));
	m_shapes.insert(m_shapes.indexOf(_after) + 1, _shape);
	if (!items().contains(_shape)) {
		addItem(_shape);
	}
	//
	// ... вставляем все вложенные карточки
	//
	Shape* lastCardInsertAfter = _shape;
	for (QGraphicsItem* childItem : _shape->childItems()) {
		if (CardShape* childCard = dynamic_cast<CardShape*>(childItem)) {
			insertShape(childCard, lastCardInsertAfter);
			lastCardInsertAfter = childCard;
		}
	}

	//
	// Фокусируем экран на неё, если это не связь
	//
	if (!dynamic_cast<Flow*>(_shape)) {
		focusShape(_shape);
	}
}

Shape* CustomGraphicsScene::takeShape(Shape* _shape, bool _removeCardFlows)
{
	if (m_shapes.contains(_shape)) {
		//
		// При необходимости соединяем между собой окружающие карточку элементы
		//
		if (_removeCardFlows) {
			//
			// ... если это карточка, конечно
			//
			if (CardShape* cardShape = dynamic_cast<CardShape*>(_shape)) {
				//
				// Определяем элементы, с которыми соединена карточка, чтобы соеденить их между собой
				//
				Flow* startFlow = cardFlow(cardShape, CARD_ON_FLOW_END);
				Flow* endFlow = nullptr;
				if (hasCards(cardShape)) {
					endFlow = cardFlow(lastCard(cardShape), CARD_ON_FLOW_START);
				} else {
					endFlow = cardFlow(cardShape, CARD_ON_FLOW_START);
				}
				//
				// ... если есть связь в оба конца, соединяем карточки на концах этих связей
				//
				if (startFlow != nullptr && endFlow != nullptr) {
					startFlow->setEndShape(endFlow->endShape());
					removeShape(endFlow);
				}
				//
				// ... если есть связь только в начале, просто уберём эту связь
				//
				else if (startFlow != nullptr) {
					removeShape(startFlow);
				}
				//
				// ... если есть связь только в конце, просто уберём эту связь
				//
				else {
					removeShape(endFlow);
				}
			}
		}

		//
		// Извлекаем фигуру
		//
		disconnect(_shape, SIGNAL(stateIsAboutToBeChangedByUser()), this, SIGNAL(stateChangedByUser()));
		m_shapes.removeAll(_shape);
		//
		// ... извлекаем все вложенные карточки
		//
		for (QGraphicsItem* childItem : _shape->childItems()) {
			if (CardShape* childCard = dynamic_cast<CardShape*>(childItem)) {
				const bool DONT_REMOVE_FLOWS = false;
				takeShape(childCard, DONT_REMOVE_FLOWS);
			}
		}
		return _shape;
	}

	return NULL;
}

void CustomGraphicsScene::removeShape(Shape* _shape)
{
	if (m_shapes.contains(_shape)) {
		//
		// Удаляем детей
		//
		for (QGraphicsItem* childItem : _shape->childItems()) {
			if (Shape* childShape = dynamic_cast<Shape*>(childItem)) {
				removeShape(childShape);
			}
		}

		//
		// Определяем внешние элементы
		//
		Shape* previousCard = nullptr;
		Shape* nextCard = nullptr;
		if (CardShape* cardShape = dynamic_cast<CardShape*>(_shape)) {
			if (Flow* startFlow = cardFlow(cardShape, CARD_ON_FLOW_END)) {
				previousCard = startFlow->startShape();
			}
			if (Flow* endFlow = cardFlow(cardShape, CARD_ON_FLOW_START)) {
				nextCard = endFlow->endShape();
			}
		}

		//
		// Удаляем сам элемент
		//
		QList<Shape *> items; items.append(_shape);
		QList<Shape *> all = m_shapes;
		bool found;
		do {
			found = false;
			for (int i = 0; i < all.count(); ++i) {
				if (Flow* item=dynamic_cast<Flow *>(all[i])) {
					if ((items.contains(item->endShape()) || items.contains(item->startShape())) && !items.contains(item)) {
						items << item, found = true;
					}
				}
			}
		} while (found);

		for(int i = items.count()-1; i>=0; --i) {
			removeItem(items[i]);
			disconnect(_shape, SIGNAL(stateIsAboutToBeChangedByUser()), this, SIGNAL(stateChangedByUser()));
			m_shapes.removeAll(items[i]);
			delete items[i];
		}

		//
		// Если надо добавляем связь между разорванными элементами
		//
		if (previousCard != nullptr && nextCard != nullptr) {
			appendShape(new ArrowFlow(previousCard, nextCard));
		}
	}
}

void CustomGraphicsScene::notifyStateChangeByUser()
{
	emit stateChangedByUser();
}

QString CustomGraphicsScene::toXML()
{
	return createSceneXml(this, (this->views().count()==0? NULL : this->views()[0]));
}

CustomGraphicsScene *CustomGraphicsScene::fromXML (const QString &xml, CustomGraphicsScene *scene)
{
	scene->removeAllShapes();
	loadSceneXml(xml, scene);
	return scene;
}

QList<Shape *> CustomGraphicsScene::selectedShapes()
{
	QList<Shape *> result;
	QList<QGraphicsItem *> items = selectedItems();
	for(int i=0; i<items.count(); ++i)
		if (m_shapes.contains(dynamic_cast<Shape *>(items[i])))
			result << dynamic_cast<Shape *>(items[i]);
	return result;
}

void CustomGraphicsScene::removeSelectedShapes()
{
	QList<Shape*> selected = selectedShapes();
	QList<Shape*> shapes;
	for (Shape* item : selected) {
		if (!dynamic_cast<Flow*>(item)) {
			shapes << dynamic_cast<Shape*>(item);
		}
	}
	for (int i = 0; i < shapes.count(); ++i) {
        //
        // Вместо удаления, перемещаем выделенные фигуры в корзину
        //
        takeShape(shapes[i]);
        m_shapesAboutToDelete << shapes[i];
	}
}

void CustomGraphicsScene::removeAllShapes()
{
	QPainterPath path;
    //
    // Вибираем все элементы
    //
	path.addRect(this->sceneRect());
	setSelectionArea(path);
    //
    // Удаляем их
    //
	removeSelectedShapes();
    //
    // Оищаем корзину
    //
    qDeleteAll(m_shapesAboutToDelete);
    m_shapesAboutToDelete.clear();

	this->clear();
	QGraphicsRectItem *item;
	addItem(item = new QGraphicsRectItem(QRectF(0,0,10000,10000)));
	item->setVisible(false);
}

void CustomGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _event)
{
	bool needSendEventToBaseClass = true;

	//
	// Если в данный момент происходит передвижение мыши с зажатой клавишей
	//
	if (_event->buttons() ^ Qt::NoButton) {
		if (selectedShapes().size() == 2) {
			//
			// Снимаем выделение со всех линий
			//
			for (Shape* shape : selectedShapes()) {
				if (Flow* flow = dynamic_cast<Flow*>(shape)) {
					flow->setSelected(false);
				}
			}
		}

		//
		// Снимаем состояние вложения со всех карточек
		//
		for (Shape* shape : shapes()) {
			if (CardShape* card = dynamic_cast<CardShape*>(shape)) {
				card->setOnInstertionState(false);
			}
		}

		//
		// Если выделена лишь одна карточка
		//
		QList<Shape*> selected = selectedShapes();
		if (selected.size() == 1) {
			if (CardShape* selectedCard = dynamic_cast<CardShape*>(selected.last())) {
				//
				// Определяем пересекающиеся с выделенным элементы
				//
				QList<QGraphicsItem*> intersectItems = items(selectedCard->mapToScene(selectedCard->shape()));
				QList<Flow*> intersectFlows;
				QList<CardShape*> containedCards;
				for (QGraphicsItem* intersectItem : intersectItems) {
					//
					// Оставляем только связи,
					//
					if (Flow* flow = dynamic_cast<Flow*>(intersectItem)) {
						//
						// ... к которым данный элемент не присоединён
						//	   и ни одна из связанных фигур не вложена в переносимую карточку
						//
						if (flow->startShape() != selectedCard
							&& flow->endShape() != selectedCard
							&& !selectedCard->isGrandParentOf(flow->startShape())
							&& !selectedCard->isGrandParentOf(flow->endShape())) {
							intersectFlows.append(flow);
						}
					}
					//
					// Или карочки,
					//
					else if (CardShape* card = dynamic_cast<CardShape*>(intersectItem)) {
						//
						// ... если они являются группирующими элементами
						//
						if (card != selectedCard
							&& (card->cardType() == CardShape::TypeScenesGroup
								|| card->cardType() == CardShape::TypeFolder)) {
							//
							// ... и перетаскиваемый элемент полностью входит в них
							//
							const QRectF selectedCardRect = selectedCard->mapToScene(selectedCard->boundingRect()).boundingRect();
							const QRectF targetCardRect = card->mapToScene(card->boundingRect()).boundingRect();
							if (targetCardRect.contains(selectedCardRect)) {
								containedCards.append(card);
							}
						}
					}
				}

				//
				// 1. Если такая связь всего одна, помечаем её, как принимающую переставление карточки
				//
				if (intersectFlows.size() == 1) {
					//
					// NOTE: чтобы избежать проблем с перерисовкой анкоров, выполняем событие
					//		 до того момента, как сделаем связь активной
					//
					QGraphicsScene::mouseMoveEvent(_event);
					intersectFlows.last()->setSelected(true);
					needSendEventToBaseClass = false;
				}
				//
				// 2. Если происходит вложение карточки в группирующую
				//
				if (!containedCards.isEmpty()) {
					//
					// Определим самый низкоуровневый элемент (тот что глубже всех вложен)
					//
					int maxDepth = -1;
					CardShape* topCard = nullptr;
					for (CardShape* card : containedCards) {
						if (maxDepth < card->depthLevel()) {
							maxDepth = card->depthLevel();
							topCard = card;
						}
					}

					//
					// Подсвечиваем его, сигнализируя о том, что вложение будет происходить именно в него
					//
					topCard->setOnInstertionState(true);
				}
			}
		}

		m_afterMoving = true;
	}

	if (needSendEventToBaseClass) {
		QGraphicsScene::mouseMoveEvent(_event);
	}
}

void CustomGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event)
{
	//
	// Обработку производим только после перемещения мыши
	//
	if (m_afterMoving) {
		//
		// 1. Перемещение карточки на связь
		//
		QList<Shape*> selected = selectedShapes();
		CardShape* selectedCard = nullptr;
		ArrowFlow* selectedFlow = nullptr;
		if (selected.size() == 2) {
			if (dynamic_cast<CardShape*>(selected.first())) {
				selectedCard = dynamic_cast<CardShape*>(selected.first());
				selectedFlow = dynamic_cast<ArrowFlow*>(selected.last());
			} else {
				selectedCard = dynamic_cast<CardShape*>(selected.last());
				selectedFlow = dynamic_cast<ArrowFlow*>(selected.first());
			}
			//
			// Если это действительно перемещение карточки на связь
			//
			if (selectedCard != nullptr
				&& selectedFlow != nullptr
				&& selectedFlow->startShape() != selectedCard
				&& selectedFlow->endShape() != selectedCard) {
				//
				// Изымаем карточку из сцены
				//
				takeShape(selectedCard);

				//
				// Определяем элементы к которым теперь будет присоединена карточка
				//
				CardShape* previousCard = dynamic_cast<CardShape*>(selectedFlow->startShape());
				CardShape* nextCard = dynamic_cast<CardShape*>(selectedFlow->endShape());
				//
				// Заменяем старую связь на новые
				//
				removeShape(selectedFlow);
				appendShape(new ArrowFlow(previousCard, selectedCard));
				if (hasCards(selectedCard)) {
					appendShape(new ArrowFlow(lastCard(selectedCard), nextCard));
				} else {
					appendShape(new ArrowFlow(selectedCard, nextCard));
				}

				//
				// Меняем порядок следования фигур
				//
				insertShape(selectedCard, previousCard);

				//
				// Если предыдущая карточка вложена в группирующий элемент
				//
				QGraphicsItem* previousParentItem = previousCard->parentItem();
				QGraphicsItem* nextParentItem = nextCard->parentItem();
				bool handled = false;
				if (previousParentItem != nullptr) {
					const QRectF selectedCardRect = selectedCard->mapToScene(selectedCard->boundingRect()).boundingRect();
					const QRectF parentCardRect = previousParentItem->mapToScene(previousParentItem->boundingRect()).boundingRect();
					//
					// ... и если текущая карточка тоже помещена внутрь группирующего элемента
					//
					if (parentCardRect.contains(selectedCardRect)) {
						//
						// ... поместим её внутрь
						//
						const QPointF lastPos = selectedCard->scenePos();
						selectedCard->setParentItem(previousParentItem);
						selectedCard->setPos(previousParentItem->mapFromScene(lastPos));
						handled = true;
					}
				}
				//
				// Если следующая карточка вложена в группирующий элемент
				//
				if (!handled
					&& nextParentItem != nullptr) {
					const QRectF selectedCardRect = selectedCard->mapToScene(selectedCard->boundingRect()).boundingRect();
					const QRectF parentCardRect = nextParentItem->mapToScene(nextParentItem->boundingRect()).boundingRect();
					//
					// ... и если текущая карточка тоже помещена внутрь группирующего элемента
					//
					if (parentCardRect.contains(selectedCardRect)) {
						//
						// ... поместим её внутрь
						//
						const QPointF lastPos = selectedCard->scenePos();
						selectedCard->setParentItem(nextParentItem);
						selectedCard->setPos(nextParentItem->mapFromScene(lastPos));
						handled = true;
					}
				}
				//
				// Если не удалось вложить, то убираем родителя у элемента
				//
				if (!handled
					&& selectedCard->parentItem() != nullptr) {
					const QPointF lastPos = selectedCard->scenePos();
					selectedCard->setParentItem(nullptr);
					selectedCard->setPos(lastPos);
				}
			}
		}

		//
		// 2. Обработка вложения и вытаскивания элементов из групп сцен и папок
		//
		selected = selectedShapes();
		if (selected.size() == 1) {
			selectedCard = dynamic_cast<CardShape*>(selected.first());
			if (selectedCard != nullptr) {
				//
				// Определим, есть ли группирующий элемент, помеченный на вложение
				//
				CardShape* parentCard = nullptr;
				for (Shape* shape : shapes()) {
					if (CardShape* card = dynamic_cast<CardShape*>(shape)) {
						if (card->isOnInstertionState()) {
							parentCard = card;
							break;
						}
					}
				}

				//
				// Если это перемещение карточки внутри своего родителя, просто снимем режим выделения
				//
				if (parentCard != nullptr
					&& selectedCard->parentItem() == parentCard) {
					parentCard->setOnInstertionState(false);
				}
				//
				// В противном случае
				//
				else {
					//
					// Вложение
					//
					if (parentCard != nullptr) {
						//
						// Изымаем карточку из сцены
						//
						takeShape(selectedCard);

						//
						// Если у группирующего элемента есть дети, то связываем с последней карточкой
						//
						if (hasCards(parentCard)) {
							//
							// Определяем элементы к которым теперь будет присоединена карточка
							//
							CardShape* previousCard = dynamic_cast<CardShape*>(lastCard(parentCard));
							Flow* previousCardFlow = cardFlow(previousCard, CARD_ON_FLOW_START);
							if (previousCardFlow != nullptr) {
								CardShape* nextCard = dynamic_cast<CardShape*>(previousCardFlow->endShape());
								//
								// Заменяем старую связь на новые
								//
								removeShape(previousCardFlow);
								if (hasCards(selectedCard)) {
									appendShape(new ArrowFlow(lastCard(selectedCard), nextCard));
								} else {
									appendShape(new ArrowFlow(selectedCard, nextCard));
								}
							}
							appendShape(new ArrowFlow(previousCard, selectedCard));

							//
							// Меняем порядок следования фигур
							//
							insertShape(selectedCard, previousCard);
						}
						//
						// Если детей нет, то связываем непосредственно с группирующим элементом
						//
						else {
							//
							// Определяем элементы к которым теперь будет присоединена карточка
							//
							CardShape* previousCard = parentCard;
							Flow* previousCardFlow = cardFlow(previousCard, CARD_ON_FLOW_START);
							if (previousCardFlow != nullptr) {
								CardShape* nextCard = dynamic_cast<CardShape*>(previousCardFlow->endShape());
								//
								// Заменяем старую связь на новые
								//
								removeShape(previousCardFlow);
								if (hasCards(selectedCard)) {
									appendShape(new ArrowFlow(lastCard(selectedCard), nextCard));
								} else {
									appendShape(new ArrowFlow(selectedCard, nextCard));
								}
							}
							appendShape(new ArrowFlow(previousCard, selectedCard));

							//
							// Меняем порядок следования фигур
							//
							insertShape(selectedCard, previousCard);
						}

						//
						// Назначаем нового родителя
						//
						const QPointF lastPos = selectedCard->scenePos();
						selectedCard->setParentItem(parentCard);
						selectedCard->setPos(parentCard->mapFromScene(lastPos));
						parentCard->setOnInstertionState(false);
					}
					//
					// Вытаскивание - соединяем с последней карточкой в сценарии
					//
					else if (selectedCard->parentItem() != nullptr) {
						//
						// Изымаем карточку из сцены
						//
						takeShape(selectedCard);

						//
						// Определяем элемент к которому теперь будет присоединена карточка
						//
						CardShape* previousCard =  dynamic_cast<CardShape*>(lastCard());
						//
						// Добавляем связь
						//
						appendShape(new ArrowFlow(previousCard, selectedCard));

						//
						// Меняем порядок следования фигур
						//
						insertShape(selectedCard, previousCard);

						//
						// Убираем родителя
						//
						const QPointF lastPos = selectedCard->scenePos();
						selectedCard->setParentItem(nullptr);
						selectedCard->setPos(lastPos);
					}
				}
			}
		}
	}

	update();

	QGraphicsScene::mouseReleaseEvent(_event);

	m_afterMoving = false;
}

void CustomGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event)
{
	if (Shape* s = dynamic_cast<Shape*>(itemAt(_event->scenePos(), QTransform()))) {
		s->editProperties();
    }
}

Shape* CustomGraphicsScene::createCard(const QString& _uuid, int _cardType, const QString& _title,
    const QString& _description, const QPointF& _scenePos, Shape* _parent)
{
    Shape* newCard = nullptr;
    //
    // Сперва пробуем восстановить из корзины
    //
    for (Shape* shape : m_shapesAboutToDelete) {
        if (CardShape* card = dynamic_cast<CardShape*>(shape)) {
            if (card->uuid() == _uuid) {
                newCard = card;
                m_shapesAboutToDelete.removeAll(card);
                break;
            }
        }
    }
    //
    // Если в корзине ничего не нашлось, создаём новую
    //
    if (newCard == nullptr) {
        newCard = new CardShape(_uuid, (CardShape::CardType)_cardType, _title, _description, _scenePos, _parent);
    }

    return newCard;
}

bool CustomGraphicsScene::hasCards(QGraphicsItem* parentItem) const
{
	//
	// Определим список, среди чего искать
	//
	QList<QGraphicsItem*> checkList;
	if (parentItem != nullptr) {
		checkList = parentItem->childItems();
	} else {
		checkList = items();
	}

	//
	// Ищем карточки
	//
	for (QGraphicsItem* item : checkList) {
		if (CardShape* card = dynamic_cast<CardShape*>(item)) {
			if (m_shapes.contains(card)) {
				return true;
			}
		}
	}

	return false;
}

Shape* CustomGraphicsScene::firstCard(Shape* _parent) const
{
	//
	// Определяем первую карточку
	//
	Shape* first = nullptr;
	for (Shape* shape : m_shapes) {
		if (shape->parentItem() == _parent
			&& dynamic_cast<CardShape*>(shape)) {
			first = shape;
			break;
		}
	}

	//
	// Если первая карточка является группирующим элементом и имеет детей, ищем первого среди них
	//
	if (CardShape* card = dynamic_cast<CardShape*>(first)) {
		if (hasCards(card)) {
			first = firstCard(card);
		}
	}

	return first;
}

Shape* CustomGraphicsScene::lastCard(Shape* _parent) const
{
	//
	// Определяем последнюю карточку
	//
	Shape* last = nullptr;
	for (Shape* shape : m_shapes) {
		if (shape->parentItem() == _parent
			&& dynamic_cast<CardShape*>(shape)) {
			last = shape;
		}
	}

	//
	// Если последняя карточка является группирующим элементом и имеет детей, ищем последнего среди них
	//
	if (CardShape* card = dynamic_cast<CardShape*>(last)) {
		if (hasCards(card)) {
			last = lastCard(card);
		}
	}

	return last;
}

Flow* CustomGraphicsScene::cardFlow(Shape* _card, bool _cardIsStartOfFlow) const
{
	Flow* flow = nullptr;
	for (Shape* shape : m_shapes) {
		if (Flow* shapeFlow = dynamic_cast<Flow*>(shape)) {
			if ((_cardIsStartOfFlow && shapeFlow->startShape() == _card) // карта в начале связи
				|| (!_cardIsStartOfFlow && shapeFlow->endShape() == _card)) { // или в конце
				flow = shapeFlow;
				break;
			}
		}
	}

	return flow;
}
