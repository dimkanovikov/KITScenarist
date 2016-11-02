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
	 * @brief Необходимо ли использовать в качестве фона пробковую доску
	 */
	void setUseCorkboardBackground(bool _use);

	/**
	 * @brief Установить цвет фона
	 */
	void setBackgroundColor(const QColor& _color);

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
	void addCard(const QString& _uuid, int _cardType, const QString& _title,
		const QString& _description, const QString& _colors, bool _isCardFirstInParent);

	/**
	 * @brief Обновить карточку с заданным номером
	 */
	void updateCard(const QString& _uuid, int _type, const QString& _title, const QString& _description, const QString& _colors);

	/**
	 * @brief Добавить заметку
	 */
	void addNote(const QString& _text);

	/**
	 * @brief Обновить текущую заметку
	 */
	void updateNote(const QString& _text);

	/**
	 * @brief Установить текст текущей связи
	 */
	void setFlowText(const QString& _text);

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
	 * @brief Сделать активной карточку с заданным uuid
	 */
	void selectCard(const QString& _uuid);

	/**
	 * @brief Получить номер выделенной карточки, если нет выделенных, или выделено больше одной, возвращается -1
	 */
	QString selectedCardUuid() const;

	/**
	 * @brief Удалить выделенные элементы
	 */
	void deleteSelectedItems();

	/**
	 * @brief Упорядочить карточки по сетке
	 */
	void arrangeCards(int _cardSize, int _cardRatio, int _distance, int _cardsInLine, bool _cardsInRow);

	/**
	 * @brief Показать контекстное меню в заданной точке для карточки
	 */
	void showContextMenu(const QPoint& _pos);

signals:
	/**
	 * @brief Запросы на добавление элементов
	 */
	/** @{ */
	void addCardRequest();
	void addFlowTextRequest();
	/** @} */

	/**
	 * @brief Запросы на изменение выделенной фигуры
	 */
	/** @{ */
	void editCardRequest(const QString& _uuid, int _cardType, const QString& _title, const QString& _color, const QString& _description);
	void editNoteRequest(const QString& _text);
	void editFlowTextRequest(const QString& _text);
	/** @} */

	/**
	 * @brief Запрос на удаление карточки
	 */
	void removeCardRequest(const QString& _uuid);

	/**
	 * @brief Карточка была перемещена
	 */
	void cardMoved(const QString& _parentUuid, const QString& _previousUuid, const QString& _movedUuid);

	/**
	 * @brief Изменились цвета карточки
	 */
	void cardColorsChanged(const QString& _uuid, const QString& _colors);

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
