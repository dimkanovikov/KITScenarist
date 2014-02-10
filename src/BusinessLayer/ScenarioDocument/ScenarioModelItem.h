#ifndef SCENARIOMODELITEM_H
#define SCENARIOMODELITEM_H

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
		ScenarioModelItem(QUuid _uuid = QUuid());
		~ScenarioModelItem();

		/**
		 * @brief Идентификатор элемента
		 */
		QUuid uuid() const;

		/**
		 * @brief Номер сцены
		 */
		int number() const;
		void setNumber(int _number);

		/**
		 * @brief Заголовок элемента
		 */
		QString header() const;
		void setHeader(const QString& _header);

		/**
		 * @brief Описание элемента
		 */
		QString synopsis() const;

		/**
		 * @brief Текст элемента
		 */
		QString text() const;

		/**
		 * @brief Установить текст элемента
		 */
		void setText(const QString& _text);

		/**
		 * @brief Длительность элемента
		 */
		int duration() const;

		/**
		 * @brief Установить длительность
		 */
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

	private:
		/**
		 * @brief Идентификатор элемента
		 */
		QUuid m_uuid;

		/**
		 * @brief Номер сцены
		 */
		int m_number;

		/**
		 * @brief Заголовок элемента
		 */
		QString m_header;

		/**
		 * @brief Описание элемента
		 */
		QString m_synopsis;

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
