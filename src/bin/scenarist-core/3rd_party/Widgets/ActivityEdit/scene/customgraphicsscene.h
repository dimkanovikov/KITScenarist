#ifndef CUSTOMGRAPHICSSCENE_H
#define CUSTOMGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>

class Flow;
class Shape;


/**
 * @class CustomGraphicsScene
 * Этот класс отвечает за все, связанное с рабочим полем:
 * за создание/удаление фигур, за управление состоянием фигур (т.е. контролем отмен),
 * за работу с мышью (создание/изменение фигур) - правда, делегирует это вспомогательным
 * структурам, за копирование выделенных фигур, за вставку фигур, и т.п.
 * В общем, он несет всю ответственность за работу с диаграммой как единым целым.
 *
 * @see Shape
 */
class CustomGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:
	/// Загружает сцену из XML
	static CustomGraphicsScene *fromXML (const QString &xml, CustomGraphicsScene *scene);
	/// Возвращает сцену, преобразованную в XML-формат
	QString toXML();

public:
	CustomGraphicsScene(QObject* _parent = 0);
	~CustomGraphicsScene();

	/**
	 * @brief Добавить новый элемент
	 */
	/** @{ */
	void appendCard(const QString& _uuid, int _cardType, const QString& _title,
		const QString& _description, bool _isCardFirstInParent);
	void appendNote(const QString& _text);
	void appendHorizontalLine();
	void appendVerticalLine();
	/** @} */

	/**
	 * @brief Получить список фигур сцены
	 */
	QList<Shape*> shapes() const;

	/**
	 * @brief Сфокусировать представление на заданной фигуре
	 */
	void focusShape(Shape* _shape);

	/**
	 * @brief Добавить фигуру на сцену
	 * @note Фигура добавляется в конец списка всех фигур сцены
	 */
	void appendShape(Shape* _shape);

	/**
	 * @brief Вставить фигуру после заданной
	 */
	void insertShape(Shape* _shape, Shape* _after);

	/**
	 * @brief Изъять фигуру
	 */
	Shape* takeShape(Shape* _shape, bool _removeCardFlows = true);

	/**
	 * @brief Удалить фигуру со сцены
	 * @note Если удаляется карточка, то внешние карточки пробуются соединиться
	 */
	void removeShape(Shape *_shape);


	void removeAllShapes();
	void removeSelectedShapes();
	QList<Shape *> selectedShapes();

signals:
	/**
	 * Испускается, когда состояние сцены изменяется пользователем.
	 * Используется для реализации многоуровневой отмены (сама сцена
	 * отмену не реализует, она лишь предоставляет средства для этого).
	 */
	void stateChangedByUser();

	/**
	 * @brief Запросы на изменение выделенной фигуры
	 */
	/** @{ */
	void editCardRequest(const QString& _uuid, int _cardType, const QString& _title, const QString& _description);
	void editNoteRequest(const QString& _text);
	void editFlowTextRequest();
	/** @} */

public slots:
	/**
	 * Предполагается, что этот слот будет активироваться извне, чтобы
	 * уведомить сцену о том, что ее состояние изменилось (например, при
	 * вставке фигур пользователем - лучше сделать это вручную, т.к., формально,
	 * сцена не должна знать, что эта операция видна пользователю, а не для
	 * каких-то служебных целей была проведена).
	 */
	void notifyStateChangeByUser();

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;

	/**
	 * @brief Переопределяем для обратотки событий вложения и вытаскивания сцен в группы и папки
	 */
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

	/**
	 * @brief Переопределяем для отображения редактора параметров элемента
	 */
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;

private:
	/**
	 * @brief Создать карточку
	 */
	Shape* createCard(const QString& _uuid, int _cardType, const QString& _title,
		const QString& _description, const QPointF& _scenePos, Shape* _parent, bool& needCorrectPosition);

	/**
	 * @brief Есть ли карточки вложенные в заданный элемент, если элемент не задан, то проверяется вся сцена
	 */
	bool hasCards(QGraphicsItem* parentItem = nullptr) const;

	/**
	 * @brief Получить первую карточку
	 */
	Shape* firstCard(Shape* _parent = nullptr) const;

	/**
	 * @brief Получить последнюю карточку
	 */
	Shape* lastCard(Shape* _parent = nullptr) const;

	/**
	 * @brief Найти связь входящую или выходящую из карты
	 */
	Flow* cardFlow(Shape* _card, bool _cardIsStartOfFlow) const;

private:
	/**
	 * @brief Фигуры сцены
	 * @note Порядок следования фигур катрочек соответствует порядку следования сцен в сценарии
	 */
	QList<Shape*> m_shapes;

	/**
	 * @brief Корзина с удалёнными фигурами
	 */
	QList<Shape*> m_shapesAboutToDelete;

	/**
	 * @brief Действие происходит после перемещения курсора
	 */
	bool m_afterMoving;
};

#endif // CUSTOMGRAPHICSSCENE_H
