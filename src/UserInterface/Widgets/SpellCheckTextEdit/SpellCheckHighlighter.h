#ifndef SPELLCHECKHIGHLIGHTER_H
#define SPELLCHECKHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class SpellChecker;


/**
 * @brief Класс подсвечивающий слова не прошедшие проверку правописания
 */
class SpellCheckHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	explicit SpellCheckHighlighter(QTextDocument* _parent, SpellChecker* _checker);

	/**
	 * @brief Включить/выключить проверку орфографии
	 */
	void setUseSpellChecker(bool _use);

protected:
	/**
	 * @brief Подсветить текст не прошедший проверку орфографии
	 */
	void highlightBlock(const QString& _text);

private:
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
};

#endif // SPELLCHECKHIGHLIGHTER_H
