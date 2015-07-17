#include "ColoredToolButton.h"

#include "ColorsPane.h"
#include "GoogleColorsPane.h"
#include "WordHighlightColorsPane.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QBitmap>
#include <QEvent>
#include <QMenu>
#include <QPainter>
#include <QWidgetAction>

ColoredToolButton::ColoredToolButton(const QIcon& _icon, QWidget* _parent) :
	QToolButton(_parent),
	m_icon(_icon),
	m_defaultColor(true),
	m_colorsPane(0)
{
	setIcon(_icon);
	setFocusPolicy(Qt::NoFocus);
	aboutUpdateIcon(palette().text().color());

	setStyleSheet("QToolButton { border: 1px solid transparent; min-width: 26px; padding: 3px; } "
				  "QToolButton[popupMode=\"1\"] { padding-right: 16px; }"
				  "QToolButton::menu-button { border: 1px solid transparent; width: 16px; }"
				  "QToolButton:hover { border: 1px solid palette(dark); }"
				  "QToolButton[popupMode=\"1\"]:hover { border-right: 1px solid transparent; }"
				  "QToolButton::menu-button:hover { border: 1px solid palette(dark); }"
				  "QToolButton:pressed, QToolButton::menu-button:pressed { background-color: palette(dark); }"
				  "QToolButton:checked, QToolButton::menu-button:checked { background-color: palette(dark); }");

	connect(this, SIGNAL(clicked()), this, SLOT(aboutClicked()));
}

ColoredToolButton::~ColoredToolButton()
{
	if (m_colorsPane != 0) {
		delete m_colorsPane;
		m_colorsPane = 0;
	}
}

void ColoredToolButton::setColorsPane(ColoredToolButton::ColorsPaneType _pane)
{
	//
	// Удаляем предыдущую панель и меню, если была
	//
	if (m_colorsPane != 0) {
		m_colorsPane->close();
		disconnect(m_colorsPane, SIGNAL(selected(QColor)), this, SLOT(setColor(QColor)));
		m_colorsPane->deleteLater();
		m_colorsPane = 0;
	}
	if (menu() != 0) {
		menu()->deleteLater();
		setMenu(0);
	}

	//
	// Создаём новую панель
	//
	switch (_pane) {
		default:
		case NonePane: {
			break;
		}

		case Google: {
			m_colorsPane = new GoogleColorsPane;
			break;
		}

		case WordHighlight: {
			m_colorsPane = new WordHighlightColorsPane;
			break;
		}
	}

	//
	// Настраиваем новую панель
	//
	if (m_colorsPane != 0) {
		setPopupMode(QToolButton::MenuButtonPopup);

		QMenu* menu = new QMenu(this);
		QWidgetAction* wa = new QWidgetAction(menu);
		wa->setDefaultWidget(m_colorsPane);
		menu->addAction(wa);
		setMenu(menu);

		connect(m_colorsPane, SIGNAL(selected(QColor)), this, SLOT(setColor(QColor)));
	} else {
		setPopupMode(QToolButton::DelayedPopup);
	}
}

QColor ColoredToolButton::currentColor() const
{
	return m_colorsPane->currentColor();
}

void ColoredToolButton::setColor(const QColor& _color)
{
	m_defaultColor = false;

	if (m_colorsPane != 0) {
		m_colorsPane->setCurrentColor(_color);
		menu()->close();
	}

	aboutUpdateIcon(_color);

	emit clicked(_color);
}

bool ColoredToolButton::event(QEvent* _event)
{
	if (m_defaultColor) {
		if (_event->type() == QEvent::PaletteChange) {
			aboutUpdateIcon(palette().text().color());
		}
	}

	return QToolButton::event(_event);
}

void ColoredToolButton::aboutUpdateIcon(const QColor& _color)
{
	QIcon newIcon = m_icon;
	ImageHelper::setIconColor(newIcon, iconSize(), _color);
	setIcon(newIcon);
}

void ColoredToolButton::aboutClicked()
{
	if (m_colorsPane != 0) {
		emit clicked(m_colorsPane->currentColor());
	}
}
