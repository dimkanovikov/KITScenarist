#include "qlightboxwidget.h"

#include <QPixmap>
#include <QEvent>
#include <QPaintEvent>
#include <QChildEvent>
#include <QPainter>

QLightBoxWidget::QLightBoxWidget(QWidget* _parent, bool _folowToHeadWidget) :
	QWidget(_parent),
	m_isInUpdateSelf(false)
{
	//
	// Родительский виджет должен быть обязательно установлен
	//
	Q_ASSERT_X(_parent, "", Q_FUNC_INFO);

	//
	// Если необходимо, делаем родителем самый "старший" виджет
	//
	if (_folowToHeadWidget) {
		while (_parent->parentWidget() != 0) {
			_parent = _parent->parentWidget();
		}
		setParent(_parent);
	}

	//
	// Следим за событиями родительского виджета, чтобы
	// иметь возможность перерисовать его, когда изменяется размер и т.п.
	//
	_parent->installEventFilter(this);

	//
	// Скрываем виджет
	//
	setVisible(false);
}

bool QLightBoxWidget::eventFilter(QObject* _object, QEvent* _event)
{
	//
	// Виджету необходимо всегда быть последним ребёнком,
	// чтобы перекрывать остальные виджеты при отображении
	//
	if (_event->type() == QEvent::ChildAdded) {
		QChildEvent* childEvent = dynamic_cast<QChildEvent*>(_event);
		if (childEvent->child() != this) {
			QWidget* parent = parentWidget();
			setParent(0);
			setParent(parent);
		}
	}

	//
	// Если изменился размер родительского виджета, необходимо
	// перерисовать себя
	//
	if (isVisible()
		&& _event->type() == QEvent::Resize) {
		updateSelf();
	}
	return QWidget::eventFilter(_object, _event);
}

void QLightBoxWidget::paintEvent(QPaintEvent* _event)
{
	//
	// Рисуем фон
	//
	QPainter p;
	p.begin(this);
	// ... фото родительского виджета
	p.drawPixmap(0, 0, width(), height(), m_parentWidgetPixmap);
	// ... накладываем затемнённую область
	p.setBrush(QBrush(QColor(0, 0, 0, 220)));
	p.drawRect(0, 0, width(), height());
	p.end();

	//
	// Рисуем всё остальное
	//
	QWidget::paintEvent(_event);
}

void QLightBoxWidget::showEvent(QShowEvent* _event)
{
	//
	// Обновим себя
	//
	updateSelf();

	//
	// Показываемся
	//
	QWidget::showEvent(_event);
}

void QLightBoxWidget::updateSelf()
{
	if (!m_isInUpdateSelf) {
		m_isInUpdateSelf = true;

		{
			//
			// Обновляем отображение
			//
			hide();
			resize(parentWidget()->size());
			m_parentWidgetPixmap = grabParentWidgetPixmap();
			show();
		}

		m_isInUpdateSelf = false;
	}
}

QPixmap QLightBoxWidget::grabParentWidgetPixmap() const
{
	QPixmap parentWidgetPixmap;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	parentWidgetPixmap = parentWidget()->grab();
#else
	parentWidgetPixmap = QPixmap::grabWidget(parentWidget());
#endif

	return parentWidgetPixmap;
}
