#include "ScenarioNavigatorItemDelegate.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QPainter>

using UserInterface::ScenarioNavigatorItemDelegate;


ScenarioNavigatorItemDelegate::ScenarioNavigatorItemDelegate(QObject* _parent) :
	QStyledItemDelegate(_parent),
	m_showSceneNumber(false),
	m_showSceneDescription(true),
	m_sceneDescriptionIsSceneText(true),
	m_sceneDescriptionHeight(1)
{
}

ScenarioNavigatorItemDelegate::~ScenarioNavigatorItemDelegate()
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
	// Определим кисти
	//
	QBrush backgroundBrush = opt.palette.background();
	QBrush textBrush = opt.palette.text();
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
	const int MARGIN = 2;
	const int RIGHT_MARGIN = 12;
	const int ITEMS_SPACING = 4;
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
	const int iconSize = m_showSceneDescription ? 32 : 20;
	const QRect iconRect(MARGIN, MARGIN, iconSize, iconSize);
	const QPixmap icon = _index.data(Qt::DecorationRole).value<QPixmap>();
	_painter->drawPixmap(iconRect, icon);
	//
	// ... длительность
	//
	const int duration = _index.data(BusinessLogic::ScenarioModel::DurationIndex).toInt();
	const QString chronometry =
			BusinessLogic::ChronometerFacade::chronometryUsed()
			? "(" + BusinessLogic::ChronometerFacade::secondsToTime(duration)+ ") "
			: "";
	const int chronometryRectWidth = _painter->fontMetrics().width(chronometry);
	const QRect chronometryRect(
		opt.rect.right() - chronometryRectWidth - ITEMS_SPACING - RIGHT_MARGIN,
		MARGIN,
		chronometryRectWidth,
		TEXT_LINE_HEIGHT
		);
	_painter->drawText(chronometryRect, chronometry);
	//
	// ... заголовок
	//
	_painter->setPen(textBrush.color());
	QFont headerFont = _painter->font();
	headerFont.setBold(m_showSceneDescription ? true : false);
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
		_painter->setPen(textBrush.color());
		QFont descriptionFont = _painter->font();
		descriptionFont.setBold(false);
		_painter->setFont(descriptionFont);
		const QRect descriptionRect(
			headerRect.left(),
			headerRect.bottom() + ITEMS_SPACING,
			chronometryRect.right() - headerRect.left(),
			TEXT_LINE_HEIGHT * m_sceneDescriptionHeight
			);
		const QString descriptionText =
				m_sceneDescriptionIsSceneText
				? _index.data(BusinessLogic::ScenarioModel::SceneTextIndex).toString()
				: _index.data(BusinessLogic::ScenarioModel::SynopsisIndex).toString();
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
	int additionalHeight = 3 + 3;
	if (m_showSceneDescription) {
		lines += m_sceneDescriptionHeight;
		additionalHeight += 2;
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
