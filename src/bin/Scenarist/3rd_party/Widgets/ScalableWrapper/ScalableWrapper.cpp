#include "ScalableWrapper.h"

#include <3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h>

#include <QApplication>
#include <QGestureEvent>
#include <QGraphicsProxyWidget>
#include <QMenu>
#include <QScreen>
#include <QScrollBar>
#include <QShortcut>
#include <QTextEdit>


ScalableWrapper::ScalableWrapper(SpellCheckTextEdit* _editor, QWidget* _parent) :
	QGraphicsView(_parent),
	m_scene(new QGraphicsScene),
	m_editor(_editor),
	m_zoomRange(1),
	m_gestureZoomInertionBreak(0)
{
	//
	// Настраиваем лучшее опции прорисовки
	//
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	//
	// Отслеживаем жесты
	//
	grabGesture(Qt::PinchGesture);

	//
	// Всегда показываем полосы прокрутки
	//
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	//
	// Предварительная настройка редактора текста
	//
	// FIXME: непонятно как быть с предком, у встраиваемого виджета не должно быть родителя,
	//		  но как в таком случае освобождать память?
	//
	m_editor->setParent(0);
	m_editor->setContextMenuPolicy(Qt::PreventContextMenu);
	m_editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_editor->installEventFilter(this);

	//
	// Настраиваем само представление
	//
	m_rect = m_scene->addRect(0, 0, 1, 1, QPen(), Qt::red);
	m_editorProxy = m_scene->addWidget(m_editor);
	setScene(m_scene);

	//
	// Отключаем действия полос прокрутки, чтобы в дальнейшем проксировать ими
	// полосы прокрутки самого редактора текста
	//
	horizontalScrollBar()->disconnect();
	verticalScrollBar()->disconnect();

	//
	// Синхронизация значения ролика в обе стороны
	//
	setupScrollingSynchronization(true);

	//
	// Добавляем возможность масштабирования при помощи комбинаций Ctrl +/-
	//
	QShortcut* zoomInShortcut1 = new QShortcut(QKeySequence("Ctrl++"), this);
	connect(zoomInShortcut1, SIGNAL(activated()), this, SLOT(zoomIn()));
	QShortcut* zoomInShortcut2 = new QShortcut(QKeySequence("Ctrl+="), this);
	connect(zoomInShortcut2, SIGNAL(activated()), this, SLOT(zoomIn()));
	QShortcut* zoomOutShortcut = new QShortcut(QKeySequence("Ctrl+-"), this);
	connect(zoomOutShortcut, SIGNAL(activated()), this, SLOT(zoomOut()));
}

SpellCheckTextEdit* ScalableWrapper::editor() const
{
	return m_editor;
}

void ScalableWrapper::setZoomRange(qreal _zoomRange)
{
	if (m_zoomRange != _zoomRange) {
		m_zoomRange = _zoomRange;
		emit zoomRangeChanged(m_zoomRange);

		scaleTextEdit();
	}
}

void ScalableWrapper::zoomIn()
{
	setZoomRange(m_zoomRange + 0.1);
}

void ScalableWrapper::zoomOut()
{
	setZoomRange(m_zoomRange - 0.1);
}

bool ScalableWrapper::event(QEvent* _event)
{
	bool result = true;
	//
	// Определяем особый обработчик для жестов
	//
	if (_event->type() == QEvent::Gesture) {
		gestureEvent(static_cast<QGestureEvent*>(_event));
	}
	//
	// Для событий showEvent и paintEvent отключаем синхронизацию полос прокрутки,
	// т.к. в стандартной реализации QGraphicsView они сбиваются для нас
	//
	else if (_event->type() == QEvent::Show
			 || _event->type() == QEvent::Paint) {
		setupScrollingSynchronization(false);

		result = QGraphicsView::event(_event);

		setupScrollingSynchronization(true);
	}
	//
	// Прочие стандартные обработчики событий
	//
	else {
		result = QGraphicsView::event(_event);

		//
		// Переустанавливаем фокус в редактор, иначе в нём пропадает курсор
		//
		if (_event->type() == QEvent::FocusIn) {
			m_editor->clearFocus();
			m_editor->setFocus();
		}
	}

	return result;
}

void ScalableWrapper::paintEvent(QPaintEvent* _event)
{
	updateTextEditSize();

	//
	// Перед прорисовкой отключаем синхронизацию полос прокрутки и отматываем полосу прокрутки
	// представление наверх, для того, чтобы не смещались координаты сцены и виджет редактора
	// не уезжал за пределы видимости обёртки
	//

	setupScrollingSynchronization(false);

	int verticalValue = m_editor->verticalScrollBar()->value();
	int horizontalValue = m_editor->horizontalScrollBar()->value();

	verticalScrollBar()->setValue(0);
	horizontalScrollBar()->setValue(0);

	QGraphicsView::paintEvent(_event);

	verticalScrollBar()->setValue(verticalValue);
	horizontalScrollBar()->setValue(horizontalValue);

	setupScrollingSynchronization(true);
}

void ScalableWrapper::wheelEvent(QWheelEvent* _event)
{
#ifdef Q_OS_MAC
	const qreal ANGLE_DIVIDER = 2.;
#else
	const qreal ANGLE_DIVIDER = 120.;
#endif
	const qreal ZOOM_COEFFICIENT_DIVIDER = 10.;

	//
	// Собственно масштабирование
	//
	if (_event->modifiers() & Qt::ControlModifier) {
		if (_event->orientation() == Qt::Vertical) {
			//
			// zoomRange > 0 - масштаб увеличивается
			// zoomRange < 0 - масштаб уменьшается
			//
			const qreal zoom = _event->angleDelta().y() / ANGLE_DIVIDER;
			setZoomRange(m_zoomRange + zoom / ZOOM_COEFFICIENT_DIVIDER);

			_event->accept();
		}
	}
	//
	// В противном случае прокручиваем редактор
	//
	else {
		QGraphicsView::wheelEvent(_event);
	}
}

void ScalableWrapper::gestureEvent(QGestureEvent* _event)
{
	if (QGesture* gesture = _event->gesture(Qt::PinchGesture)) {
		if (QPinchGesture* pinch = qobject_cast<QPinchGesture *>(gesture)) {
			//
			// При масштабировании за счёт жестов приходится немного притормаживать
			// т.к. события приходят слишком часто и при обработке каждого события
			// пользователю просто невозможно корректно настроить масштаб
			//

			const int INERTION_BREAK_STOP = 8;
			qreal zoomDelta = 0;
			if (pinch->scaleFactor() > 1) {
				if (m_gestureZoomInertionBreak < 0) {
					m_gestureZoomInertionBreak = 0;
				} else if (m_gestureZoomInertionBreak >= INERTION_BREAK_STOP) {
					m_gestureZoomInertionBreak = 0;
					zoomDelta = 0.1;
				} else {
					++m_gestureZoomInertionBreak;
				}
			} else if (pinch->scaleFactor() < 1) {
				if (m_gestureZoomInertionBreak > 0) {
					m_gestureZoomInertionBreak = 0;
				} else if (m_gestureZoomInertionBreak <= -INERTION_BREAK_STOP) {
					m_gestureZoomInertionBreak = 0;
					zoomDelta = -0.1;
				} else {
					--m_gestureZoomInertionBreak;
				}
			} else {
				//
				// При обычной прокрутке часто приходит событие с scaledFactor == 1,
				// так что просто игнорируем их
				//
			}

			//
			// Если необходимо масштабируем и перерисовываем представление
			//
			if (zoomDelta != 0) {
				setZoomRange(m_zoomRange + zoomDelta);
			}

			_event->accept();
		}
	}
}

bool ScalableWrapper::eventFilter(QObject* _object, QEvent* _event)
{
	bool needShowMenu = false;
	QPoint cursorPos = QCursor::pos();
	switch (_event->type()) {
		case QEvent::ContextMenu: {
			QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(_event);
			cursorPos = m_editor->viewport()->mapFromGlobal(contextMenuEvent->globalPos());
			needShowMenu = true;
			break;
		}

		case QEvent::MouseButtonPress: {
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(_event);
			if (mouseEvent->button() == Qt::RightButton) {
				cursorPos = m_editor->viewport()->mapFromGlobal(mouseEvent->globalPos());
				needShowMenu = true;
			}
			break;
		}

		default: {
			break;
		}
	}

	bool result = false;

	//
	// Если необходимо, то показываем контекстное меню в отдельном прокси элементе,
	// предварительно вернув ему 100% масштаб
	//
	if (needShowMenu) {
		QMenu* menu = m_editor->createContextMenu(cursorPos);
		QGraphicsProxyWidget* menuProxy = m_editorProxy->createProxyForChildWidget(menu);

		const qreal antiZoom = 1. / m_zoomRange;
		menuProxy->setScale(antiZoom);
		menuProxy->setPos(QCursor::pos());

		//
		// Если меню вываливается за экран, корректируем его позицию
		//
		QRectF menuRect(QCursor::pos(), menu->sizeHint());
		//
		// ... по вертикали
		//
		if (menuRect.bottom() > QApplication::primaryScreen()->size().height()) {
			menuRect.moveBottom(menuRect.top());
		}
		//
		// ... по горизонтали
		//
		if (menuRect.right() > QApplication::primaryScreen()->size().width()) {
			menuRect.moveRight(menuRect.left());
		}
		menuProxy->setPos(menuRect.topLeft());

		menu->exec();
		delete menu;

		m_editor->clearFocus();
		m_editor->setFocus();

		//
		// Событие перехвачено
		//
		result = true;
	}
	//
	// Если нет, то стандартная обработка события
	//
	else {
		result = QGraphicsView::eventFilter(_object, _event);
	}

	return result;
}

void ScalableWrapper::setupScrollingSynchronization(bool _needSync)
{
	if (_needSync) {
		connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
				m_editor->verticalScrollBar(), SLOT(setValue(int)));
		connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
				m_editor->horizontalScrollBar(), SLOT(setValue(int)));
		// --
		connect(m_editor->verticalScrollBar(), SIGNAL(valueChanged(int)),
				verticalScrollBar(), SLOT(setValue(int)));
		connect(m_editor->horizontalScrollBar(), SIGNAL(valueChanged(int)),
				horizontalScrollBar(), SLOT(setValue(int)));
	} else {
		disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)),
				m_editor->verticalScrollBar(), SLOT(setValue(int)));
		disconnect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
				m_editor->horizontalScrollBar(), SLOT(setValue(int)));
		// --
		disconnect(m_editor->verticalScrollBar(), SIGNAL(valueChanged(int)),
				verticalScrollBar(), SLOT(setValue(int)));
		disconnect(m_editor->horizontalScrollBar(), SIGNAL(valueChanged(int)),
				horizontalScrollBar(), SLOT(setValue(int)));
	}
}

void ScalableWrapper::updateTextEditSize()
{
	//
	// Размер редактора устанавливается таким образом, чтобы скрыть масштабированные полосы
	// прокрутки (скрывать их нельзя, т.к. тогда теряются значения, которые необходимо проксировать)
	//
	const int editorWidth =
			viewport()->width() / m_zoomRange
			+ m_editor->verticalScrollBar()->width() + m_zoomRange;
	const int editorHeight =
			viewport()->height() / m_zoomRange
			+ m_editor->horizontalScrollBar()->height() + m_zoomRange;
	m_editorProxy->resize(editorWidth, editorHeight);

	//
	// Необходимые действия для корректировки значений на полосах прокрутки
	//
	const int rectWidth = m_editor->horizontalScrollBar()->maximum();
	const int rectHeight = m_editor->verticalScrollBar()->maximum();

	m_rect->setRect(0, 0, rectWidth, rectHeight);

	horizontalScrollBar()->setMaximum(rectWidth);
	verticalScrollBar()->setMaximum(rectHeight);
}

void ScalableWrapper::scaleTextEdit()
{
	const qreal DEFAULT_ZOOM_RANGE = 1.;
	const qreal MINIMUM_ZOOM_RANGE = 0.5;
	if (m_zoomRange == 0) {
		m_zoomRange = DEFAULT_ZOOM_RANGE;
	} else if (m_zoomRange < MINIMUM_ZOOM_RANGE) {
		m_zoomRange = MINIMUM_ZOOM_RANGE;
	}
	m_editorProxy->setScale(m_zoomRange);
}

