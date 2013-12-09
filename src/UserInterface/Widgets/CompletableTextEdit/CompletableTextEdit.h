#ifndef COMPLETABLETEXTEDIT_H
#define COMPLETABLETEXTEDIT_H

#include <UserInterface/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h>

class QCompleter;
class QAbstractItemModel;


/**
 * @brief Класс текстового редактора с подстановщиком для завершения текста
 */
class CompletableTextEdit : public SpellCheckTextEdit
{
	Q_OBJECT

public:
	explicit CompletableTextEdit(QWidget* _parent = 0);

	/**
	 * @brief Получить подстановщика
	 */
	QCompleter* completer() const;

	/**
	 * @brief Открыт ли подстановщик
	 */
	bool isCompleterVisible() const;

	/**
	 * @brief Показать автодополнение текста
	 * @return Есть ли в модели для дополнения элементы с таким текстом
	 */
	bool complete(QAbstractItemModel* _model, const QString& _completionPrefix);

	/**
	 * @brief Применить выбранный в подстановщике элемент
	 */
	void applyCompletion();

	/**
	 * @brief Закрыть подстановщика, если открыт
	 */
	void closeCompleter();

private:
	/**
	 * @brief Подстановщик для завершения текста
	 */
	QCompleter* m_completer;
};

#endif // COMPLETABLETEXTEDIT_H
