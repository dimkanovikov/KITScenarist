#include "FlatButton.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QEvent>
#include <QPainter>

namespace {
	/**
	 * @brief Размер иконки
	 */
	static QSize ICON_SIZE(20, 20);
}


FlatButton::FlatButton(QWidget* _parent) :
	QToolButton(_parent),
	m_checkedIconHighlight(true)
{
	setIconSize(ICON_SIZE);

	connect(this, SIGNAL(toggled(bool)), this, SLOT(aboutUpdateIcon()));
}

void FlatButton::setIcons(const QIcon& _icon, const QIcon& _checkedIcon, const QIcon& _hoverIcon)
{
	m_icon = _icon;
	ImageHelper::setIconColor(m_icon, ICON_SIZE, palette().text().color());

	m_checkedIcon = _checkedIcon;
	if (!m_checkedIcon.isNull()) {
		m_checkedIconHighlight = false;
		ImageHelper::setIconColor(m_checkedIcon, ICON_SIZE, palette().text().color());
	} else {
		m_checkedIconHighlight = true;
		m_checkedIcon = _icon;
		QColor highlightColor = palette().highlight().color();
		ImageHelper::setIconColor(m_checkedIcon, ICON_SIZE, highlightColor);
	}

	m_hoverIcon = _hoverIcon;

	aboutUpdateIcon();
}

void FlatButton::updateIcons()
{
	if (m_icon.isNull()) {
		setIcons(icon(), m_checkedIcon, m_hoverIcon);
	}
}

bool FlatButton::event(QEvent* _event)
{
	if (_event->type() == QEvent::PaletteChange) {
		ImageHelper::setIconColor(m_icon, ICON_SIZE, palette().text().color());
		if (!m_checkedIconHighlight) {
			ImageHelper::setIconColor(m_checkedIcon, ICON_SIZE, palette().text().color());
		}

		aboutUpdateIcon();
	}

	return QToolButton::event(_event);
}

void FlatButton::enterEvent(QEvent* _event)
{
	QToolButton::enterEvent(_event);

	if (!m_icon.isNull()) {
		if (!m_hoverIcon.isNull()) {
			const bool isHoverIcon = isEnabled();
			setIcon(isHoverIcon ? m_hoverIcon : m_icon);
		} else if (!m_checkedIcon.isNull()) {
			const bool isCheckedIcon = isEnabled() && isChecked();
			setIcon(isCheckedIcon ? m_checkedIcon : m_icon);
		}
	}
}

void FlatButton::leaveEvent(QEvent* _event)
{
	QToolButton::leaveEvent(_event);

	if (!m_icon.isNull()) {
		const bool isCheckedIcon = !m_checkedIcon.isNull() && isEnabled() && isChecked();
		setIcon(isCheckedIcon ? m_checkedIcon : m_icon);
	}
}

void FlatButton::aboutUpdateIcon()
{
	if (!m_icon.isNull()) {
		const bool isCheckedIcon = !m_checkedIcon.isNull() && isEnabled() && isChecked();
		setIcon(isCheckedIcon ? m_checkedIcon : m_icon);
	}
}

