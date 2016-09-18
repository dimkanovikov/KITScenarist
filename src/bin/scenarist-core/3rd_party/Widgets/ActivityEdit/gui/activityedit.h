#ifndef ACTIVITYEDIT_H
#define ACTIVITYEDIT_H

#include <QFrame>

class CustomGraphicsView;
class SceneUndoStack;


/**
 * @brief Класс редактора диаграм
 * @note Включает в себя всю логику по работе с редактором
 */
class ActivityEdit : public QFrame
{
	Q_OBJECT

public:
	explicit ActivityEdit(QWidget *parent = 0);
	~ActivityEdit();

	/**
	 * @brief Очистить схему
	 */
	void clear();

	/**
	 * @brief Отменить последнее действие
	 */
	void undo();

	/**
	 * @brief Повторить последнее действие
	 */
	void redo();

	/**
	 * @brief Сохранить схему в XML-строку
	 */
	QString save() const;

	/**
	 * @brief Загрузить схему из XMl-строки
	 */
	void load(const QString& _xml);

	/**
	 * @brief Добавить карточку
	 */
	void addCard(int _cardType, const QString& _title, const QString& _description);

	/**
	 * @brief Обновить карточку с заданным номером
	 */
	void updateCard(int _cardNumber, int _type, const QString& _title, const QString& _description);

	/**
	 * @brief Добавить заметку
	 */
	void addNote(const QString& _text);

	/**
	 * @brief Добавить горизонтальную линию
	 */
	void addHorizontalLine();

	/**
	 * @brief Добавить вертикальную линию
	 */
	void addVerticalLine();

	/**
	 * @brief Выделить все элементы схемы
	 */
	void selectAll();

	/**
	 * @brief Сделать активной карточку с заданным номером
	 */
	void selectCard(int _cardNumber);

	/**
	 * @brief Получить номер выделенной карточки, если нет выделенных, или выделено больше одной, возвращается -1
	 */
	int selectedCardNumber() const;

	/**
	 * @brief Удалить выделенные элементы
	 */
	void deleteSelectedItems();

signals:
	/**
	 * @brief Схема изменена
	 */
	void schemeChanged();

private:
	/**
	 * @brief Представление редактора карточек
	 */
	CustomGraphicsView* m_view;

	/**
	 * @brief Список последних изменений
	 */
	SceneUndoStack* m_undoStack;
};

#endif // ACTIVITYEDIT_H
