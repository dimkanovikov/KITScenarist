#include "ScenarioNavigatorItemDelegate.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Helpers/StyleSheetHelper.h>

#include <QPainter>

using UserInterface::ScenarioNavigatorItemDelegate;

namespace {
	/**
	 * @brief Отступы
	 */
	static int margin() {
		return StyleSheetHelper::dpToPx(
#ifndef MOBILE_OS
					3
#else
					9
#endif
					);
	}

	/**
	 * @brief Расстояние между строками
	 */
	static int spacing() {
		return StyleSheetHelper::dpToPx(
#ifndef MOBILE_OS
					4
#else
					8
#endif
					);
	}
}


ScenarioNavigatorItemDelegate::ScenarioNavigatorItemDelegate(QObject* _parent) :
	QStyledItemDelegate(_parent),
	m_showSceneNumber(false),
	m_showSceneDescription(true),
	m_sceneDescriptionIsSceneText(true),
	m_sceneDescriptionHeight(1)
{
}

void ScenarioNavigatorItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
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
	QBrush backgroundBrush = opt.palette.background();
	QBrush textBrush = opt.palette.text();
	QFont headerFont = opt.font;
	headerFont.setBold(m_showSceneDescription ? true : false);
	QFont textFont = opt.font;
	textFont.setBold(false);
	//
	// ... для выделенных элементов
	//
	if(opt.state.testFlag(QStyle::State_Selected))
	{
		backgroundBrush = opt.palette.highlight();
		textBrush = opt.palette.highlightedText();
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
			backgroundBrush = opt.palette.alternateBase();
			textBrush = opt.palette.windowText();
		}
		else
		{
			backgroundBrush = opt.palette.base();
			textBrush = opt.palette.windowText();
		}
	}

	//
	// Рисуем
	//
	const int TREE_INDICATOR_WIDTH = StyleSheetHelper::dpToPx(18);
	const int COLOR_RECT_WIDTH = StyleSheetHelper::dpToPx(12);
	const int MARGIN = ::margin();
	const int RIGHT_MARGIN = StyleSheetHelper::dpToPx(12);
	const int ITEMS_SPACING = ::spacing();
	const int TEXT_LINE_HEIGHT = _painter->fontMetrics().height();
	//
	// ... фон
	//
	_painter->fillRect(opt.rect, backgroundBrush);
	//
	// Меняем координаты, чтобы рисовать было удобнее
	//
	_painter->translate(opt.rect.topLeft());
	//
	// ... иконка
	//
	const int iconSize =  StyleSheetHelper::dpToPx(20);
	const int iconTopMargin = MARGIN;
	const QRect iconRect(MARGIN, iconTopMargin, iconSize, iconSize);
	QPixmap icon = _index.data(Qt::DecorationRole).value<QPixmap>();
	QIcon iconColorized(icon);
	QColor iconColor = textBrush.color();
	// ... если есть заметка, рисуем красноватым цветом
	if (_index.data(BusinessLogic::ScenarioModel::HasNoteIndex).toBool()) {
		iconColor = QColor("#ec3838");
	}
	ImageHelper::setIconColor(iconColorized, iconRect.size(), iconColor);
	icon = iconColorized.pixmap(iconRect.size());
	_painter->drawPixmap(iconRect, icon);
	//
	// ... цвета сцены
	//
	const QString colorsNames = _index.data(BusinessLogic::ScenarioModel::ColorIndex).toString();
	QStringList colorsNamesList = colorsNames.split(";", QString::SkipEmptyParts);
	const int maxColorsInColumn = m_showSceneDescription ? m_sceneDescriptionHeight + 1 : 1;
	int colorsCount = colorsNamesList.size();
	int colorRectX = TREE_INDICATOR_WIDTH + opt.rect.width() - COLOR_RECT_WIDTH - ITEMS_SPACING - RIGHT_MARGIN;
	while (colorsCount > 0) {
		//
		// Выссчитываем сколько влезет в одну колонку
		//
		int colorsInColumn = 0;
		if (colorsCount >= maxColorsInColumn) {
			colorsInColumn = maxColorsInColumn;
		} else {
			colorsInColumn = colorsCount;
		}

		//
		// Рисуем, начиная с последних выбранных цветов
		//
		for (int colorIndex = 0; colorIndex < colorsInColumn; ++colorIndex) {
			const QString colorName = colorsNamesList.takeAt(colorsCount - colorsInColumn);
			const QColor color(colorName);
			const QRect colorRect(
						colorRectX,
						opt.rect.height() / colorsInColumn * colorIndex,
						COLOR_RECT_WIDTH,
						opt.rect.height() / colorsInColumn
						);
			_painter->fillRect(colorRect, color);
		}

		//
		// Обновляем счётчик оставшихся цветов
		//
		colorsCount = colorsNamesList.size();

		//
		// Сдвигаем координату для прорисовку предыдущих цветов
		//
		if (colorsCount > 0) {
			colorRectX -= COLOR_RECT_WIDTH;
		}
	}
	//
	// ... текстовая часть
	//
	_painter->setPen(textBrush.color());
	//
	// ... длительность
	//
	_painter->setFont(textFont);
	const int duration = _index.data(BusinessLogic::ScenarioModel::DurationIndex).toInt();
	const QString chronometry =
			BusinessLogic::ChronometerFacade::chronometryUsed()
			? "(" + BusinessLogic::ChronometerFacade::secondsToTime(duration)+ ") "
			: "";
	const int chronometryRectWidth = _painter->fontMetrics().width(chronometry);
	const QRect chronometryRect(
		colorRectX - chronometryRectWidth - ITEMS_SPACING,
		MARGIN,
		chronometryRectWidth,
		TEXT_LINE_HEIGHT
		);
	_painter->drawText(chronometryRect, chronometry);
	//
	// ... заголовок
	//
	_painter->setFont(headerFont);
	const QRect headerRect(
		iconRect.right() + ITEMS_SPACING,
		MARGIN,
		chronometryRect.left() - iconRect.right() - ITEMS_SPACING*2,
		TEXT_LINE_HEIGHT
		);
	QString header = _index.data(Qt::DisplayRole).toString().toUpper();
	if (m_showSceneNumber) {
		//
		// Если нужно добавляем номер сцены
		//
		QVariant sceneNumber = _index.data(BusinessLogic::ScenarioModel::SceneNumberIndex);
		if (!sceneNumber.isNull()) {
			header = sceneNumber.toString() + ". " + header;
		}
	}
	header = _painter->fontMetrics().elidedText(header, Qt::ElideRight, headerRect.width());
	_painter->drawText(headerRect, header);
	//
	// ... описание
	//
	if (m_showSceneDescription) {
		_painter->setFont(textFont);
		const QRect descriptionRect(
			headerRect.left(),
			headerRect.bottom() + ITEMS_SPACING,
			chronometryRect.right() - headerRect.left(),
			TEXT_LINE_HEIGHT * m_sceneDescriptionHeight
			);
		const QString descriptionText =
				m_sceneDescriptionIsSceneText
				? _index.data(BusinessLogic::ScenarioModel::SceneTextIndex).toString()
				: _index.data(BusinessLogic::ScenarioModel::DescriptionIndex).toString();
		_painter->drawText(descriptionRect, Qt::TextWordWrap, descriptionText);
	}

	_painter->restore();
}

QSize ScenarioNavigatorItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	Q_UNUSED(_option);
	Q_UNUSED(_index);

	//
	// Размер составляется из лейблов
	// строка на заголовок и m_sceneDescriptionHeight строк на описание
	// + отступы 3 сверху + 3 снизу + 2 между текстом
	//
	int lines = 1;
	int additionalHeight = ::margin() + ::margin();
	if (m_showSceneDescription) {
		lines += m_sceneDescriptionHeight;
		additionalHeight += ::spacing();
	}
	const int height = _option.fontMetrics.height() * lines + additionalHeight;
	const int width = 50;
	return QSize(width, height);
}

void ScenarioNavigatorItemDelegate::setShowSceneNumber(bool _show)
{
	if (m_showSceneNumber != _show) {
		m_showSceneNumber = _show;
	}
}

void ScenarioNavigatorItemDelegate::setShowSceneDescription(bool _show)
{
	if (m_showSceneDescription != _show) {
		m_showSceneDescription = _show;
	}
}

void ScenarioNavigatorItemDelegate::setSceneDescriptionIsSceneText(bool _isSceneText)
{
	if (m_sceneDescriptionIsSceneText != _isSceneText) {
		m_sceneDescriptionIsSceneText = _isSceneText;
	}
}

void ScenarioNavigatorItemDelegate::setSceneDescriptionHeight(int _height)
{
	if (m_sceneDescriptionHeight != _height) {
		m_sceneDescriptionHeight = _height;
	}
}
