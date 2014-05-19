#include "PagesTextEdit.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QScrollBar>


PagesTextEdit::PagesTextEdit(QWidget *parent) :
	QTextEdit(parent),
	m_usePageMode(false),
	m_charsInLine(0),
	m_linesInPage(0)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

bool PagesTextEdit::usePageMode() const
{
	return m_usePageMode;
}

void PagesTextEdit::setUsePageMode(bool _use)
{
	if (m_usePageMode != _use) {
		//
		// Если необходимо установить режим использования постраничного вывода, то
		// обязательно должен быть задан размер страницы
		//
		if (_use
			&& m_charsInLine > 0
			&& m_linesInPage > 0) {
			m_usePageMode = true;
		}
		//
		// Для установки режима сплошного отображения ни каких ограничений нет
		// поэтому устанавливаем его в любом случае
		//
		else if (!_use) {
			m_usePageMode = false;
		}

		//
		// Перерисуем себя
		//
		repaint();
	}
}

void PagesTextEdit::setPageSize(int _charsInLine, int _linesInPage)
{
	if (m_charsInLine != _charsInLine
		|| m_linesInPage != _linesInPage) {
		m_charsInLine = _charsInLine;
		m_linesInPage = _linesInPage;
	}
}

void PagesTextEdit::paintEvent(QPaintEvent* _event)
{
	updateInnerGeometry();

	paintPagesView();

	QTextEdit::paintEvent(_event);
}

void PagesTextEdit::updateInnerGeometry()
{
	//
	// Формируем параметры отображения
	//
	QSizeF documentSize(width() - verticalScrollBar()->width(), -1);
	QMargins viewportMargins;

	if (m_usePageMode) {
		//
		// Настроить размер документа
		// Учитываются:
		// - кол-во символов в строке
		// - кол-во строк в документе
		// - отступы документа
		//

		QFontMetrics fm(document()->defaultFont());
		int pageWidth = (document()->documentMargin() * 2) + (fm.width("W") * m_charsInLine);
		int pageHeight = (document()->documentMargin() * 2) + (fm.lineSpacing() * m_linesInPage) + 2;

		documentSize = QSizeF(pageWidth, pageHeight);

		//
		// Рассчитываем отступы для viewport
		//
		if (width() > pageWidth) {
			viewportMargins =
					QMargins(
						(width() - pageWidth - verticalScrollBar()->width() - 2)/2,
						20,
						(width() - pageWidth - verticalScrollBar()->width() - 2)/2,
						20);
		} else {
			viewportMargins = QMargins(0, 20, 0, 20);
		}
	}

	//
	// Применяем параметры отображения
	//

	if (document()->pageSize() != documentSize) {
		document()->setPageSize(documentSize);
	}

	setViewportMargins(viewportMargins);
}

void PagesTextEdit::paintPagesView()
{
	//
	// Оформление рисуется только тогда, когда редактор находится в постраничном режиме
	//
	if (m_usePageMode) {
		//
		// Нарисовать линии разрыва страниц
		//

		QFontMetrics fm(document()->defaultFont());
        int pageWidth = (document()->documentMargin() * 2) + (fm.width("W") * m_charsInLine) + 2;
		int pageHeight = (document()->documentMargin() * 2) + (fm.lineSpacing() * m_linesInPage) + 2;

		QPainter p(viewport());
		QPen spacePen(palette().window(), 9);
		QPen borderPen(palette().dark(), 1);

		int curHeight = pageHeight - (verticalScrollBar()->value() % pageHeight);

		//
		// Нарисовать верхнюю границу
		//
		if (curHeight - pageHeight >= 0) {
			p.setPen(borderPen);
			// ... верхняя
			p.drawLine(0, curHeight - pageHeight, pageWidth, curHeight - pageHeight);
		}

		while (curHeight < height()) {
			//
			// Фон разрыва страниц
			//
			p.setPen(spacePen);
			p.drawLine(0, curHeight-4, width(), curHeight-4);

			//
			// Границы страницы
			//
			p.setPen(borderPen);
			// ... нижняя
			p.drawLine(0, curHeight-8, pageWidth, curHeight-8);
			// ... верхняя следующей страницы
			p.drawLine(0, curHeight, pageWidth, curHeight);
			// ... левая
			p.drawLine(0, curHeight-pageHeight, 0, curHeight-8);
			// ... правая
            p.drawLine(pageWidth-1, curHeight-pageHeight, pageWidth-1, curHeight-8);

			curHeight += pageHeight;
		}

		//
		// Нарисовать боковые границы страницы, когда страница не влезает в экран
		//
		if (curHeight >= height()) {
			//
			// Границы страницы
			//
			p.setPen(borderPen);
			// ... левая
			p.drawLine(0, curHeight-pageHeight, 0, height());
			// ... правая
            p.drawLine(pageWidth-1, curHeight-pageHeight, pageWidth-1, height());
		}
	}
}
