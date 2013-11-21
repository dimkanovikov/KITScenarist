#ifndef SPELLCHECKTEXTEDIT_H
#define SPELLCHECKTEXTEDIT_H

#include <QTextEdit>

class SpellChecker;
class SpellCheckHighlighter;


/**
 * @brief Класс текстового редактора с проверкой орфографии
 */
class SpellCheckTextEdit : public QTextEdit
{
	Q_OBJECT
public:
	explicit SpellCheckTextEdit(QWidget *parent = 0);

protected:
	/**
	 * @brief Получить путь к файлу с пользовательским словарём
	 * @return Путь к файлу со словарём
	 *
	 * @note В текущей реализации возвращается пустая строка.
	 */
	virtual QString userDictionaryfile() const;

private:
	/**
	 * @brief Проверяющий орфографию
	 */
	SpellChecker* m_spellChecker;

	/**
	 * @brief Подсвечивающий орфографические ошибки
	 */
	SpellCheckHighlighter* m_spellCheckHighlighter;

};

#endif // SPELLCHECKTEXTEDIT_H
