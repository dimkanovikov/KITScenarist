#include "textutils.h"


QSizeF TextUtils::textRect(const QString& text, const QFont& font, int width, const QTextOption& option)

{
	QTextLayout textLayout(text, font);
	QFontMetricsF metrics(font);
	int leading = metrics.leading();
	qreal height = 0;
	textLayout.setTextOption(option);
	textLayout.beginLayout();
	int length = 0;
	while(true)
	{
		QTextLine line = textLayout.createLine();
		if (!line.isValid()) break;
		line.setLineWidth(width);
		height += leading + line.height();
		length += line.textLength();
	}
	textLayout.endLayout();
	height += metrics.height() * text.count("\n");
	return QSizeF(width, height);
}

QString TextUtils::elidedText(const QString& text, const QFont& font, const QSizeF& sz, const QTextOption& option)
{
	/*
	 Сначала определим длину текста, который умещается в прямоугольнике
	 Для этого найдем количество строк, которые там умещаются (+ еще переводы!)
	 И затем суммируем их длины
	*/
	QTextLayout textLayout(text, font);
	QFontMetricsF metrics(font);
	int leading = metrics.leading();
	qreal height = 0;
	textLayout.setTextOption(option);
	textLayout.beginLayout();
	int length = 0;
	while(true)
	{
		QTextLine line = textLayout.createLine();
		if (!line.isValid()) break;
		line.setLineWidth(sz.width());
		height += leading + line.height();
		if (height>sz.height()) break;
		length += line.textLength();
	}
	textLayout.endLayout();
	QString str = text.left(length); // Это наша искомая строка, но без многоточия

	/*
	 Теперь нужно найти, сколько текста поместится вместе с многоточием.
	 Следует учитывать, что, если текст можно отобразить полностью,
	 многоточия не требуется.
	*/
	QRectF shaperect = QRectF(0,0,sz.width(),sz.height());
	QRectF shaperecth = QRectF(0,0,sz.width(),sz.height()*2); // очень высокий, чтобы можно было узнать реальную высоту текста.
	while(true)
	{
		// мало ли, вдруг зацикливание
		if (str.length()==0) break;

		// Выполняем цикл, пока текст не помещается в прямоугольник rect
		QRectF rect = metrics.boundingRect(
						  shaperecth,
						  Qt::AlignLeft | Qt::TextWordWrap,
						  str + (str.length()==text.length()? "" : "..."));
		if (shaperect.contains(rect))
			break;

		// Отрезаем один символ и снова будем пытаться уместить его
		str = str.left(str.length()-1);
	}

	// собственно вот! ради чего все затевалось
	return str + (str.length()==text.length()? "" : "..."); // и так сойдет
}
