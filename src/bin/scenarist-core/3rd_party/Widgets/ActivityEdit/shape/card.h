#ifndef CARD_SHAPE_H
#define CARD_SHAPE_H

#include "resizableshape.h"


/**
 * @brief Карточка сценария
 */
class CardShape : public ResizableShape
{
	Q_OBJECT

public:
	/**
	 * @brief Тип карточки
	 */
	enum CardType {
		TypeScene,
		TypeScenesGroup,
		TypeFolder
	};

public:
	CardShape(QGraphicsItem* _parent = NULL);
	CardShape(CardType _type, const QString& _title, const QString& _description,
		const QPointF& _pos, QGraphicsItem* _parent = NULL);

	/**
	 * @brief Тип
	 */
	/** @{ */
	void setCardType(CardType _type);
	CardType cardType() const;
	/** @} */

	/**
	 * @brief Заголовок
	 */
	/** @{ */
	void setTitle(const QString& _title);
	QString title() const;
	/** @} */

	/**
	 * @brief Описание
	 */
	/** @{ */
	void setDescription(const QString& _description);
	QString description() const;
	/** @} */

	/**
	 * @brief Установить состояние вложения дочернего элемента
	 */
	/** @{ */
	void setOnInstertionState(bool _on);
	bool isOnInstertionState() const;
	/** @} */

	/**
	 * @brief Уровень вложенности
	 */
	int depthLevel() const;

	/**
	 * @brief Является ли карточка предком заданного элемента
	 */
	bool isGrandParentOf(QGraphicsItem* _item) const;

	/**
	 * @brief Подогнать размер фигуры (если он меньше минимального, то увеличить).
	 */
	void adjustSize() override;

	virtual void editProperties();

protected:
	/**
	 * @brief Рисуем карточку
	 */
	void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

private:
	/**
	 * @brief Тип карточки
	 */
	CardType m_cardType;

	/**
	 * @brief Заголовок карточки
	 */
	QString m_title;

	/**
	 * @brief Описание карточки
	 */
	QString m_description;

	/**
	 * @brief Карточка находится в состоянии вложения дочернего элемента
	 */
	bool m_isOnInsertionState;
};

#endif // CARD_SHAPE_H
