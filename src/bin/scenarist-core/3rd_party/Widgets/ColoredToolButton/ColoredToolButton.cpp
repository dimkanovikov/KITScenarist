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
	m_colorNotChoosedYet(true),
	m_colorsPane(0)
{
    setIcon(_icon);
    setFocusPolicy(Qt::NoFocus);
    aboutUpdateIcon(palette().text().color());

    connect(this, SIGNAL(clicked()), this, SLOT(aboutClicked()));
}

ColoredToolButton::ColoredToolButton(QWidget* _parent) :
    QToolButton(_parent),
    m_colorNotChoosedYet(true),
    m_colorsPane(0)
{
    setFocusPolicy(Qt::NoFocus);

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
	QColor result;
	if (m_colorNotChoosedYet == false) {
		result = m_colorsPane->currentColor();
	}
	return result;
}

void ColoredToolButton::setColor(const QColor& _color)
{
    updateColor(_color);

    emit clicked(_color);
}

void ColoredToolButton::updateColor(const QColor& _color)
{
    QColor newColor;
    if (_color.isValid()) {
        m_colorNotChoosedYet = false;
        newColor = _color;
    } else {
        m_colorNotChoosedYet = true;
        newColor = palette().text().color();
    }

    if (m_colorsPane != 0
        && m_colorsPane->contains(_color)) {
        m_colorsPane->setCurrentColor(newColor);
        menu()->close();
    }

    aboutUpdateIcon(_color);
}

bool ColoredToolButton::event(QEvent* _event)
{
	if (m_colorNotChoosedYet) {
		if (_event->type() == QEvent::PaletteChange) {
			aboutUpdateIcon(palette().text().color());
		}
	}

	return QToolButton::event(_event);
}

void ColoredToolButton::aboutUpdateIcon(const QColor& _color)
{
    //
    // Если иконка ещё не была сохранена, делаем это
    //
    if (m_icon.isNull()) {
        m_icon = icon();
    }

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
