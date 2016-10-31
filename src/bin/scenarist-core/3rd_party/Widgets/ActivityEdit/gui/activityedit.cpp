#include "activityedit.h"

#include "customgraphicsview.h"
#include "../scene/customgraphicsscene.h"
#include "../scene/sceneundostack.h"
#include "../shape/card.h"
#include "../shape/note.h"
#include "../flow/arrowflow.h"
#include "../xml/load_xml.h"
#include "../xml/save_xml.h"

#include <3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QMenu>
#include <QVBoxLayout>
#include <QWidgetAction>


ActivityEdit::ActivityEdit(QWidget *parent) :
	QFrame(parent),
	m_view(new CustomGraphicsView(this)),
	m_undoStack(new SceneUndoStack)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_view->setDragMode(QGraphicsView::RubberBandDrag);
	m_view->setRubberBandSelectionMode(Qt::ContainsItemShape);
	m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

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
    connect(m_view, &CustomGraphicsView::contextMenuRequest, this, &ActivityEdit::showContextMenu);

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
	const QString& _description, const QString& _colors, bool _isCardFirstInParent)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		m_view->setDragMode(QGraphicsView::NoDrag);
		scene->appendCard(_uuid, (CardShape::CardType)_cardType, _title, _description, _colors, _isCardFirstInParent);
		scene->notifyStateChangeByUser();
	}
}

void ActivityEdit::updateCard(const QString& _uuid, int _type, const QString& _title,
	const QString& _description, const QString& _colors)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		for (auto shape : scene->shapes()) {
			if (CardShape* currentCard = dynamic_cast<CardShape*>(shape)) {
				if (currentCard->uuid() == _uuid) {
					currentCard->setCardType((CardShape::CardType)_type);
					currentCard->setTitle(_title);
					currentCard->setDescription(_description);
					currentCard->setColors(_colors);
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

void ActivityEdit::arrangeCards(int _cardSize, int _cardRatio, int _distance, int _cardsInLine, bool _cardsInRow)
{
	if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
		//
		// Если в схеме есть папки, то сообщаем о том, что такую схему не получится упорядочить
		//
		auto shapes = scene->shapes();
		for (auto shape : shapes) {
			if (!shape->childItems().isEmpty()) {
				QLightBoxMessage::information(this, tr("Can't arrange cards"),
					tr("Schemes with scene groups or with folders cannot be arranged."));
				return;
			}
		}

		//
		// Сперва убираем все существующие узлы у связей
		//
		for (auto shape : shapes) {
			if (Flow* flow = dynamic_cast<Flow*>(shape)) {
				flow->removeAllFlowKnots();
			}
		}

		//
		// Вычисляем размер карточки
		//
		qreal widthDivider = 1;
		qreal heightDivider = 1;
		switch (_cardRatio) {
			case 0: heightDivider = 0.2; break; // 5x1
			case 1: heightDivider = 0.4; break; // 5x2
			case 2: heightDivider = 0.6; break; // 5x3
			case 3: heightDivider = 0.8; break; // 5x4
			case 4: break; // 5x5
			case 5: widthDivider = 0.8; break; // 4x5
			case 6: widthDivider = 0.6; break; // 3x5
			case 7: widthDivider = 0.4; break; // 2x5
			case 8: widthDivider = 0.2; break; // 1x5
		}
		const qreal CARD_WIDTH = (qreal)_cardSize * widthDivider;
		const qreal CARD_HEIGHT = (qreal)_cardSize * heightDivider;
		const QSizeF CARD_SIZE(CARD_WIDTH, CARD_HEIGHT);

		//
		// Располагаем карточки в нужном направлении
		//
		const qreal START_POS = 100.0;
		qreal x = START_POS;
		qreal y = START_POS;
		const int FIRST_CARD_INDEX = 1; // начинаем с одного, чтобы исключить -1 в условии ниже
		int currentCardInLine = FIRST_CARD_INDEX;
		for (auto shape : shapes) {
			if (CardShape* card = dynamic_cast<CardShape*>(shape)) {
				card->setSize(CARD_SIZE);
				card->setPos(x, y);

				//
				// Если это первая карточка ряда, то настроим узлы линии соединяющей текущую карточку с предыдущей
				//
				if (currentCardInLine == FIRST_CARD_INDEX
					&& (x != START_POS || y != START_POS)) {
					//
					// ... рассчитаем узловые точки
					//
					QPointF firstPoint, secondPoint;
					if (_cardsInRow) {
						firstPoint.setX(START_POS + (_cardsInLine - 1) * (_distance + CARD_WIDTH) + CARD_WIDTH / 2.0);
						firstPoint.setY(y - _distance / 2.0);
						secondPoint.setX(START_POS + CARD_WIDTH / 2.0);
						secondPoint.setY(firstPoint.y());
					} else {
						firstPoint.setX(x - _distance / 2.0);
						firstPoint.setY(START_POS + (_cardsInLine - 1) * (_distance + CARD_HEIGHT) + CARD_HEIGHT / 2.0);
						secondPoint.setX(firstPoint.x());
						secondPoint.setY(START_POS + CARD_HEIGHT / 2.0);
					}
					//
					// ... настроим узлы
					//
					for (auto shape : shapes) {
						if (Flow* flow = dynamic_cast<Flow*>(shape)) {
							if (flow->endShape() == card) {
								flow->setFlowKnots({ firstPoint, secondPoint });
								break;
							}
						}
					}
				}

				//
				// Если следующую карту нужно расположить в этой же линии
				//
				if (currentCardInLine < _cardsInLine) {
					++currentCardInLine;
					if (_cardsInRow) {
						x += CARD_WIDTH + _distance;
					} else {
						y += CARD_HEIGHT + _distance;
					}
				}
				//
				// Если переходим к следующей линии
				//
				else {
					currentCardInLine = FIRST_CARD_INDEX;
					if (_cardsInRow) {
						x = START_POS;
						y += CARD_HEIGHT + _distance;
					} else {
						x += CARD_WIDTH + _distance;
						y = START_POS;
					}
				}
			}
		}

		//
		// Обновляем экран, чтобы убрать все артефакты
		//
		scene->update();

		emit schemeChanged();
    }
}

void ActivityEdit::showContextMenu(const QPoint& _pos)
{
    if (CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(m_view->scene())) {
        if (!scene->selectedShapes().isEmpty()
            && scene->selectedShapes().size() == 1) {
            //
            // Если это запрос на контекстное меню для карточки
            //
            if (CardShape* card = dynamic_cast<CardShape*>(scene->selectedShapes().first())) {
                QMenu* menu = new QMenu(this);
                //
                // Цвета
                //
                QString colorsNames = card->colors();
                int colorIndex = 1;
                QList<GoogleColorsPane*> colorsPanesList;
                //
                // ... добавляем каждый цвет
                //
                foreach (const QString& colorName, colorsNames.split(";", QString::SkipEmptyParts)) {
                    QAction* color = menu->addAction(tr("Color %1").arg(colorIndex));
                    QMenu* colorMenu = new QMenu(this);
                    QAction* removeColor = colorMenu->addAction(tr("Remove"));
                    removeColor->setData(QString("removeColor:%1").arg(colorIndex));
                    QWidgetAction* wa = new QWidgetAction(colorMenu);
                    GoogleColorsPane* colorsPane = new GoogleColorsPane(colorMenu);
                    colorsPane->setCurrentColor(QColor(colorName));
                    wa->setDefaultWidget(colorsPane);
                    colorMenu->addAction(wa);
                    color->setMenu(colorMenu);

                    connect(colorsPane, SIGNAL(selected(QColor)), menu, SLOT(close()));

                    colorsPanesList.append(colorsPane);

                    ++colorIndex;
                }
                //
                // ... пункт для нового цвета
                //
                {
                    QAction* color = menu->addAction(tr("Add color"));
                    QMenu* colorMenu = new QMenu(this);
                    QWidgetAction* wa = new QWidgetAction(colorMenu);
                    GoogleColorsPane* colorsPane = new GoogleColorsPane(colorMenu);
                    wa->setDefaultWidget(colorsPane);
                    colorMenu->addAction(wa);
                    color->setMenu(colorMenu);

                    connect(colorsPane, SIGNAL(selected(QColor)), menu, SLOT(close()));

                    colorsPanesList.append(colorsPane);
                }

                //
                // Остальное
                //
                menu->addSeparator();
                QAction* addNew = menu->addAction(tr("Create Card After"));
                QAction* remove = menu->addAction(tr("Remove"));

                //
                // Выводим меню
                //
                QAction* toggled = menu->exec(mapToGlobal(_pos));
                if (toggled != 0) {
                    if (toggled->data().toString().startsWith("removeColor")) {
                        //
                        // Удаляем выбранный цвет из списка и обновляемся
                        //
                        const int removeColorIndex = toggled->data().toString().split(":").last().toInt();
                        QString newColorsNames;
                        int colorIndex = 1;
                        foreach (const QString& colorName, colorsNames.split(";", QString::SkipEmptyParts)) {
                            if (colorIndex != removeColorIndex) {
                                if (!newColorsNames.isEmpty()) {
                                    newColorsNames.append(";");
                                }
                                newColorsNames.append(colorName);
                            }

                            ++colorIndex;
                        }

                        emit cardColorsChanged(card->uuid(), newColorsNames);
                    } else if (toggled == addNew) {
                        emit addCardRequest();
                    } else if (toggled == remove) {
                        emit removeCardRequest(card->uuid());
                    }
                } else {
                    //
                    // Добавляем новый цвет и обновляемся
                    //
                    QString newColorsNames;
                    foreach (GoogleColorsPane* colorsPane, colorsPanesList) {
                        if (colorsPane->currentColor().isValid()) {
                            if (!newColorsNames.isEmpty()) {
                                newColorsNames.append(";");
                            }
                            newColorsNames.append(colorsPane->currentColor().name());
                        }
                    }
                    emit cardColorsChanged(card->uuid(), newColorsNames);
                }

                menu->deleteLater();
            }
        }
    }
}
