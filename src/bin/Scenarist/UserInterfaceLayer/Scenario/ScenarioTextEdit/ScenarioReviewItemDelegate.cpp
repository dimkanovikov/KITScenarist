#include "ScenarioReviewItemDelegate.h"

#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <QAbstractItemView>
#include <QApplication>
#include <QDateTime>
#include <QLabel>
#include <QPainter>

using UserInterface::ScenarioReviewItemDelegate;
using BusinessLogic::ScenarioReviewModel;

namespace {
	/**
	 * @brief Ширина цветовой метки
	 */
	const int COLOR_MARK_WIDTH = 12;

	/**
	 * @brief Расстояния
	 */
	const int TOP_MARGIN = 3, SPACING = 6, BOTTOM_MARGIN = 0, RIGHT_MARGIN = 3;

	/**
	 * @brief Рассчитать высоту текста по заданной ширине
	 */
	static int heightForWidth(const QString& _text, int _width) {
		static QLabel s_label;
		s_label.setWordWrap(true);

		int height = 0;
		if (!_text.isEmpty()) {
			s_label.setText(_text);
			height = s_label.heightForWidth(_width);
		}
		return height;
	}

	/**
	 * @brief Рассчитать высоту комментария по заданной ширине области элемента
	 */
	static int commentHeightForWidth(const QString& _text, int _width) {
		//
		// Рассчитаем ширину, которую займёт комментарий
		//
		const int commentWidth = _width - COLOR_MARK_WIDTH - SPACING - RIGHT_MARGIN;
		return heightForWidth(_text, commentWidth);
	}
}


int ScenarioReviewItemDelegate::commentIndexFor(const QModelIndex& _index, int _y, QWidget* _widget)
{
	int width = _widget->width();
	if (const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(_widget)) {
		width = view->viewport()->width();
	}

	//
	// Считаем высоту
	//
	// ... высота заголовка: отступ сверху + две строки (автор и дата) + отступ снизу
	//
	const int headerHeight = TOP_MARGIN + _widget->fontMetrics().height() * 2 + BOTTOM_MARGIN;
	//
	// ... полная высота
	//
	int height = 0;
	if (_index.data(ScenarioReviewModel::IsDoneRole).toBool() == false) {
		const QStringList comments = _index.data(ScenarioReviewModel::CommentsRole).toStringList();
		foreach (const QString& comment, comments) {
			const int top = height;
			height += headerHeight + ::commentHeightForWidth(comment, width) + SPACING;
			const int bottom = height;

			if (top <= _y && _y <= bottom) {
				return comments.indexOf(comment);
			}
		}
	}

	return 0;
}

ScenarioReviewItemDelegate::ScenarioReviewItemDelegate(QObject* _parent) :
	QStyledItemDelegate(_parent)
{
}

void ScenarioReviewItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	//
	// Получим настройки стиля
	//
	QStyleOptionViewItemV4 opt = _option;
	initStyleOption(&opt, _index);

	//
	// Рисуем ручками
	//
	_painter->save();
	_painter->setRenderHint(QPainter::Antialiasing, true);

	//
	// Определим кисти и шрифты
	//
	QColor backgroundColor = opt.palette.background().color();
	QColor replyBackgroundColor = opt.palette.window().color();
	QColor borderColor = opt.palette.midlight().color();
	QColor textColor = opt.palette.text().color();
	QColor replyTextColor = textColor;
	QColor dateColor = opt.palette.dark().color();
	QFont headerFont = opt.font;
	headerFont.setBold(true);
	QFont dateFont = opt.font;
	dateFont.setBold(true);
#ifdef Q_OS_WIN
	dateFont.setPointSize(dateFont.pointSize() - 1);
#else
	dateFont.setPointSize(dateFont.pointSize() - 4);
#endif
	QFont textFont = QApplication::font();
	//
	// ... для выделенных элементов
	//
	if(opt.state.testFlag(QStyle::State_Selected))
	{
		backgroundColor = opt.palette.highlight().color();
		textColor = opt.palette.highlightedText().color();
	}
	//
	// ... для остальных
	//
	else
	{
		//
		// Реализация альтернативных цветов в представлении
		//
		if(opt.features.testFlag(QStyleOptionViewItemV2::Alternate))
		{
			backgroundColor = opt.palette.alternateBase().color();
			textColor = opt.palette.windowText().color();
		}
		else
		{
			backgroundColor = opt.palette.base().color();
			textColor = opt.palette.windowText().color();
		}
	}

	//
	// Рисуем
	//
	const int HEADER_LINE_HEIGHT = QFontMetrics(headerFont).height();
	const int DATE_LINE_HEIGHT = QFontMetrics(dateFont).height();
	//
	// Меняем координаты, чтобы рисовать было удобнее
	//
	_painter->translate(opt.rect.topLeft());

	//
	// Определим данные
	//
	const bool done  = _index.data(ScenarioReviewModel::IsDoneRole).toBool();
	QStringList authors = _index.data(ScenarioReviewModel::CommentsAuthorsRole).toStringList();
	const QStringList dates = _index.data(ScenarioReviewModel::CommentsDatesRole).toStringList();
	const QStringList comments = _index.data(ScenarioReviewModel::CommentsRole).toStringList();
	//
	// ... если не задано ни одного автора, то эмулируем одного, для формирования пустой строки
	//
	if (authors.isEmpty()) {
		authors.append("");
	}

	//
	// Для каждого комментария
	//
	int width = _option.widget->width();
	if (const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(_option.widget)) {
		width = view->viewport()->width();
	}
	const int headerHeight = TOP_MARGIN + _option.fontMetrics.height() * 2 + BOTTOM_MARGIN;
	int lastTop = 0;
	for (int commentIndex = 0; commentIndex < authors.size(); ++commentIndex) {
		//
		// Определим область комментария
		//
		int height = headerHeight;
		if (!done) {
			height += ::commentHeightForWidth(comments.value(commentIndex), width) + SPACING;
		}
		const QRect rect(0, lastTop, width, height);

		//
		// ... фон
		//
		_painter->fillRect(rect, commentIndex == 0 ? backgroundColor : replyBackgroundColor);

		//
		// ... цвет заметки
		//
		const QColor color = _index.data(Qt::DecorationRole).value<QColor>();
		const QRect colorRect(0, lastTop, COLOR_MARK_WIDTH, height);
		_painter->fillRect(colorRect, commentIndex == 0 ? color : replyBackgroundColor);

		//
		// ... границы
		//
		_painter->setPen(borderColor);
		_painter->drawRect(rect);

		//
		// ... автор
		//
		_painter->setPen(commentIndex == 0 ? textColor : replyTextColor);
		_painter->setFont(headerFont);
		const QRect headerRect(
			colorRect.right() + SPACING,
			lastTop + TOP_MARGIN,
			width - colorRect.right() - SPACING - RIGHT_MARGIN,
			HEADER_LINE_HEIGHT
			);
		_painter->drawText(headerRect, authors.value(commentIndex));

		//
		// ... дата
		//
		_painter->setPen(dateColor);
		_painter->setFont(dateFont);
		const QRect dateRect(
			colorRect.right() + SPACING,
			headerRect.bottom(),
			width - colorRect.right() - SPACING - RIGHT_MARGIN,
			DATE_LINE_HEIGHT
			);
		const QString date = QDateTime::fromString(dates.value(commentIndex), Qt::ISODate).toString("dd.MM.yyyy hh:mm");
		_painter->drawText(dateRect, date);

		//
		// ... если комментарий исправлен на этом месте завершаем рисование
		//
		if (done) {
			break;
		}

		//
		// ... комментарий
		//
		_painter->setPen(commentIndex == 0 ? textColor : replyTextColor);
		_painter->setFont(textFont);
		const QRect commentRect(
			colorRect.right() + SPACING,
			dateRect.bottom() + SPACING,
			width - colorRect.right() - SPACING - RIGHT_MARGIN,
			height - headerHeight - SPACING
			);
		_painter->drawText(commentRect, Qt::TextWordWrap, comments.value(commentIndex));


		lastTop += height;
	}

	_painter->restore();
}

QSize ScenarioReviewItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	QSize size = QStyledItemDelegate::sizeHint(_option, _index);

	if (_option.widget != 0) {
		//
		// Ширина
		//
		int width = _option.widget->width();
		if (const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(_option.widget)) {
			width = view->viewport()->width();
		}

		//
		// Считаем высоту
		//
		// ... высота заголовка: отступ сверху + две строки (автор и дата) + отступ снизу
		//
		const int headerHeight = TOP_MARGIN + _option.fontMetrics.height() * 2 + BOTTOM_MARGIN;
		//
		// ... полная высота
		//
		int height = 0;
		if (_index.data(ScenarioReviewModel::IsDoneRole).toBool() == false) {
			const QStringList comments = _index.data(ScenarioReviewModel::CommentsRole).toStringList();
			foreach (const QString& comment, comments) {
				height += headerHeight + ::commentHeightForWidth(comment, width) + SPACING;
			}
		}
		if (height == 0) {
			height = headerHeight;
		}

		size = QSize(width, height);
	}

	return size;
}

