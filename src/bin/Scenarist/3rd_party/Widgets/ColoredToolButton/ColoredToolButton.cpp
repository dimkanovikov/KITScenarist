#include "ColoredToolButton.h"

#include "ColorsPane.h"
#include "GoogleColorsPane.h"
#include "WordHighlightColorsPane.h"

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
		None: {
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
	QPixmap baseIconPixmap = m_icon.pixmap(iconSize());
	QPixmap newIconPixmap = baseIconPixmap;

	QPainter painter(&newIconPixmap);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.fillRect(newIconPixmap.rect(), _color);
	painter.end();

	setIcon(QIcon(newIconPixmap));
}

void ColoredToolButton::aboutClicked()
{
	if (m_colorsPane != 0) {
		emit clicked(m_colorsPane->currentColor());
	}
}
