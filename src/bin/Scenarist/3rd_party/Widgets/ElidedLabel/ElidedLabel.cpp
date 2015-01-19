#include "ElidedLabel.h"

#include <QDebug>
#include <QPainter>
#include <QResizeEvent>
#include <QTextLayout>

//---------------------------------------------------------------------

ElidedLabel::ElidedLabel(QWidget *parent, Qt::WindowFlags f)
	: QLabel(parent, f)
	, elideMode_(Qt::ElideRight)
{
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
}

ElidedLabel::ElidedLabel(const QString &txt, QWidget *parent, Qt::WindowFlags f)
	: QLabel(txt, parent, f)
	, elideMode_(Qt::ElideRight)
{
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
}

ElidedLabel::ElidedLabel(const QString &txt,
						 Qt::TextElideMode elideMode,
						 QWidget *parent,
						 Qt::WindowFlags f)
	: QLabel(txt, parent, f)
	, elideMode_(elideMode)
{
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
}

//---------------------------------------------------------------------

void ElidedLabel::setText(const QString &txt) {
	QLabel::setText(txt);
	cacheElidedText(geometry().width());
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
}

//---------------------------------------------------------------------

void ElidedLabel::cacheElidedText(int w) {
	//
	// Вручную
	//
	const int LINE_LIMIT = height() / fontMetrics().height();

	//
	// Одна строка
	//
	if (LINE_LIMIT == 1) {
		cachedElidedText = fontMetrics().elidedText(text(), elideMode_, w, Qt::TextShowMnemonic);
	}
	//
	// Много строк
	//
	else {
		//
		// Определяем необходимую для сокращения ширину текста в зависимости от количества строк
		//
		static QTextLayout textLayout;
		textLayout.setText(text());
		int widthUsed = 0;
		int lineCount = 0;
		textLayout.beginLayout();
		const int LINE_LIMIT = height() / fontMetrics().height();

		while (++lineCount < LINE_LIMIT) {
			QTextLine line = textLayout.createLine();
			if (!line.isValid())
				break;

			line.setLineWidth(w);
			widthUsed += line.naturalTextWidth();
		}
		textLayout.endLayout();

		widthUsed += w;

		cachedElidedText = fontMetrics().elidedText(text(), elideMode_, widthUsed, Qt::TextShowMnemonic);
	}
}

//---------------------------------------------------------------------

void ElidedLabel::resizeEvent(QResizeEvent *e) {
	QLabel::resizeEvent(e);
	cacheElidedText(e->size().width());
}

//---------------------------------------------------------------------

void ElidedLabel::paintEvent(QPaintEvent *e) {
	if(elideMode_ == Qt::ElideNone) {
		QLabel::paintEvent(e);
	} else {
		QPainter p(this);
		p.drawText(0, 0,
				   geometry().width(),
				   geometry().height(),
				   alignment() | Qt::TextWordWrap,
				   cachedElidedText);
	}
}

//---------------------------------------------------------------------
