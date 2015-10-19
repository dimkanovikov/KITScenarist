#ifndef SPELLCHECKHIGHLIGHTER_H
#define SPELLCHECKHIGHLIGHTER_H

#include "SyntaxHighlighter.h"

class SpellChecker;


/**
 * @brief Класс подсвечивающий слова не прошедшие проверку правописания
 */
class SpellCheckHighlighter : public SyntaxHighlighter
{
	Q_OBJECT

public:
	explicit SpellCheckHighlighter(QTextDocument* _parent, SpellChecker* _checker);

	/**
	 * @brief Установить проверяющего
	 */
	void setSpellChecker(SpellChecker* _checker);

	/**
	 * @brief Включить/выключить проверку орфографии
	 */
	void setUseSpellChecker(bool _use);

	/**
	 * @brief Включена ли проверка орфографии
	 */
	bool useSpellChecker() const;

protected:
	/**
	 * @brief Подсветить текст не прошедший проверку орфографии
	 */
	void highlightBlock(const QString& _text);

private:
	/**
	 * @brief Проверяющий орфографию
	 */
	SpellChecker* m_spellChecker;

	/**
	 * @brief Формат текста не прошедшего проверку орфографии
	 */
	QTextCharFormat m_misspeledCharFormat;

	/**
	 * @brief Использовать проверяющего
	 */
	bool m_useSpellChecker;

	/**
	 * @brief Слово для очистки подсветки
	 */
	QString m_wordForClean;
};

#endif // SPELLCHECKHIGHLIGHTER_H
