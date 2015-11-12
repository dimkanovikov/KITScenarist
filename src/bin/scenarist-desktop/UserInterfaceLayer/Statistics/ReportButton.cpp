#include "ReportButton.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QIcon>
#include <QPainter>
#include <QVariant>

namespace {
	/**
	 * @brief Отступ справа
	 */
	const int RIGHT_MARGIN = 8;

	/**
	 * @brief Размер иконки
	 */
	const QSize ICON_SIZE(20, 20);
}

using UserInterface::ReportButton;


ReportButton::ReportButton(const QString& _text, BusinessLogic::StatisticsParameters::Type _type, int _subtype,
	QWidget* _parent) :
	QRadioButton(_text, _parent),
	m_type(_type),
	m_subtype(_subtype)
{
	init();
}

BusinessLogic::StatisticsParameters::Type ReportButton::type() const
{
	return m_type;
}

int ReportButton::subtype() const
{
	return m_subtype;
}

void ReportButton::paintEvent(QPaintEvent* _event)
{
	QRadioButton::paintEvent(_event);

	//
	// Рисуем шеврон
	//
	if (isChecked() || underMouse()) {
		QPainter painter(this);

		const QRect chevronRect(
			QPoint(width() - RIGHT_MARGIN - ICON_SIZE.width(), (height() - ICON_SIZE.height()) / 2),
				ICON_SIZE);
		QIcon chevronIcon(":/Graphics/Icons/chevron-right.png");
		const QColor iconColor = isChecked() ? palette().highlightedText().color() : palette().text().color();
		ImageHelper::setIconColor(chevronIcon, ICON_SIZE, iconColor);

		painter.drawPixmap(chevronRect, chevronIcon.pixmap(ICON_SIZE));
	}
}

void ReportButton::init()
{
	setProperty("chevron", true);
}

