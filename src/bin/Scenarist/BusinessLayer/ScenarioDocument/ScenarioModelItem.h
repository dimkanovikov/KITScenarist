#ifndef SCENARIOMODELITEM_H
#define SCENARIOMODELITEM_H

#include <BusinessLayer/Counters/Counter.h>

#include <QUuid>
#include <QPixmap>


namespace BusinessLogic
{
	/**
	 * @brief Класс элемента модели сценария
	 */
	class ScenarioModelItem
	{
	public:
		/**
		 * @brief Перечисление типов элементов
		 */
		enum Type {
			Undefined,
			Scene,
			SceneGroup,
			Folder,
			Scenario
		};

	public:
		ScenarioModelItem(int _position);
		~ScenarioModelItem();

		/**
		 * @brief Позиция элемента
		 */
		int position() const;
		void setPosition(int _position);

		/**
		 * @brief Номер сцены
		 */
		int sceneNumber() const;
		void setSceneNumber(int _number);

		/**
		 * @brief Заголовок элемента
		 */
		QString header() const;
		void setHeader(const QString& _header);

		/**
		 * @brief Цвет элемента
		 */
		QColor color() const;
		void setColor(const QColor& _color);

		/**
		 * @brief Описание элемента
		 */
		QString description() const;
		void setDescription(const QString& _description);

		/**
		 * @brief Текст элемента
		 */
		QString text() const;
		void setText(const QString& _text);

		/**
		 * @brief Длительность элемента
		 */
		int duration() const;
		void setDuration(int _duration);

		/**
		 * @brief Тип элемента
		 */
		Type type() const;
		void setType(Type _type);

		/**
		 * @brief Иконка объекта
		 */
		QPixmap icon() const;

		/**
		 * @brief Имеется ли в элементе примечание
		 */
		bool hasNote() const;
		void setHasNote(bool _hasNote);

		/**
		 * @brief Количество слов элемента
		 */
		Counter counter() const;
		void setCounter(const Counter& _counter);

	private:
		/**
		 * @brief Обновить текст элемента
		 *
		 * @note Для элементов группирующих в себе подэлементы
		 */
		void updateParentText();

		/**
		 * @brief Обновить длительность
		 *
		 * @note Для элементов группирующих в себе подэлементы
		 */
		void updateParentDuration();

		/**
		 * @brief Обновить счётчики
		 *
		 * @note Для элементов группирующих в себе подэлементы
		 */
		void updateParentCounter();

		/**
		 * @brief Очистить элемент
		 */
		void clear();

	private:
		/**
		 * @brief Позиция элемента в тексте
		 */
		int m_position;

		/**
		 * @brief Номер сцены
		 */
		int m_sceneNumber;

		/**
		 * @brief Заголовок элемента
		 */
		QString m_header;

		/**
		 * @brief Цвет элемента
		 */
		QColor m_color;

		/**
		 * @brief Описание элемента
		 */
		QString m_description;

		/**
		 * @brief Текст элемента
		 */
		QString m_text;

		/**
		 * @brief Длительность элемента
		 */
		int m_duration;

		/**
		 * @brief Тип элемента
		 */
		Type m_type;

		/**
		 * @brief Имеется ли в элементе примечание
		 */
		bool m_hasNote;

		/**
		 * @brief Счётчик слов и сиволов
		 */
		Counter m_counter;

	/**
	 * @brief Вспомогательные методы для организации работы модели
	 */
	/** @{ */
	public:
		/**
		 * @brief Добавить элемент в начало
		 */
		void prependItem(ScenarioModelItem* _item);

		/**
		 * @brief Добавить элемент в конец
		 */
		void appendItem(ScenarioModelItem* _item);

		/**
		 * @brief Вставить элемент в указанное место
		 */
		void insertItem(int _index, ScenarioModelItem* _item);

		/**
		 * @brief Удалить элемент
		 */
		void removeItem(ScenarioModelItem* _item);

		/**
		 * @brief Имеет ли элемент родительский элемент
		 */
		bool hasParent() const;

		/**
		 * @brief Родительский элемент
		 */
		ScenarioModelItem* parent() const;

		/**
		 * @brief Дочерний элемент по индексу
		 */
		ScenarioModelItem* childAt(int _index) const;

		/**
		 * @brief Индекс дочернего элемента
		 */
		int rowOfChild(ScenarioModelItem* _child) const;

		/**
		 * @brief Количество дочерних элементов
		 */
		int childCount() const;

		/**
		 * @brief Имеет ли элемент детей
		 */
		bool hasChildren() const;

	private:
		/**
		 * @brief Родительский элемент
		 */
		ScenarioModelItem* m_parent;

		/**
		 * @brief Дочерние элементы
		 */
		QList<ScenarioModelItem*> m_children;

	/** @} */
	};
}



#endif // SCENARIOMODELITEM_H
