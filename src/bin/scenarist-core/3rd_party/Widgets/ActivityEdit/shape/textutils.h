#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include <QFontMetrics>
#include <QTextLayout>
#include <QTextOption>


class TextUtils
{
public:
	/**
	 * Возвращает минимальный прямоугольник, в который помещается текст.
	 * @param text Текст
	 * @param font Шрифт, которым рисуется текст
	 * @param width Ширина фигуры (она останется неизменной)
	 * @param option Параметры отображения
	 * @return Размер прямоугольника.
	 */
	static QSizeF textRect(const QString &text, const QFont &font, int width, const QTextOption &option);

	/**
	 * Возвращает сокращенный текст, такой, чтобы не превышал указанных размеров при рендеринге.
	 * @param text Текст, который нужно сократить.
	 * @param font Шрифт, которым выводится текст.
	 * @param sz Максимальные размеры текста.
	 * @param option Всякие опции для текста. Будут использованы в QTextLayout.
	 * @returns Сокращенный текст (с многоточием на конце).
	 *          Если весь помещается - то весь и будет возвращен.
	 */
	static QString elidedText(const QString &text, const QFont &font, const QSizeF &sz, const QTextOption &option);
};

#endif // TEXTUTILS_H
