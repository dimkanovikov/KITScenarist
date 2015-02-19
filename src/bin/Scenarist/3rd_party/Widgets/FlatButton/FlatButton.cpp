#include "FlatButton.h"


FlatButton::FlatButton(QWidget* _parent) :
	QToolButton(_parent)
{
	setIconSize(QSize(20, 20));

	connect(this, SIGNAL(toggled(bool)), this, SLOT(aboutUpdateIcon()));
}

void FlatButton::setIcons(const QIcon& _icon, const QIcon& _hoverIcon, const QIcon& _checkedIcon)
{
	m_icon = _icon;
	m_hoverIcon = _hoverIcon;
	m_checkedIcon = _checkedIcon;

	setIcon(_icon);
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

