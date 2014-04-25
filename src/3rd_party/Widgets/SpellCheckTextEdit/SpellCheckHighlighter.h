#ifndef SPELLCHECKHIGHLIGHTER_H
#define SPELLCHECKHIGHLIGHTER_H

#include <QObject>

#include <QTextCharFormat>

class SpellChecker;
class QTextDocument;


/**
 * @brief Класс подсвечивающий слова не прошедшие проверку правописания
 */
class SpellCheckHighlighter : public QObject
{
	Q_OBJECT

public:
	explicit SpellCheckHighlighter(QTextDocument* _parent, SpellChecker* _checker);

	/**
	 * @brief Включить/выключить проверку орфографии
	 */
	void setUseSpellChecker(bool _use);

	/**
	 * @brief Перепроверить весь текст
	 */
	void rehighlight();

private slots:
	/**
	 * @brief Подсветить текст не прошедший проверку орфографии
	 */
	void highlightBlock(int _from, int _charsRemoved, int _charsAdded);

private:
	/**
	 * @brief Родительский документ
	 */
	QTextDocument* document() const;

	/**
	 * @brief Подсветить текст не прошедший проверку орфографии
	 */
	void highlightBlock(const QString& _text);

	/**
	 * @brief Установить формат текста не прошедшего проверку
	 */
	void setFormat(int _start, int _count, const QTextCharFormat& _format);

private:
	/**
	 * @brief Документ, в котором проверяется орфография
	 */
	QTextDocument* m_document;

	/**
	 * @brief Проверяющий орфографию
	 */
	SpellChecker* m_spellchecker;

	/**
	 * @brief Формат текста не прошедшего проверку орфографии
	 */
	QTextCharFormat m_misspeledCharFormat;

	/**
	 * @brief Использовать проверяющего
	 */
	bool m_useSpellChecker;

	/**
	 * @brief Флаг состояния подсветки
	 */
	bool m_inHighlight;

	/**
	 * @brief Позиция начала проверки
	 */
	int m_start;
};

#endif // SPELLCHECKHIGHLIGHTER_H
