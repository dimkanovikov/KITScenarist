#ifndef RESEARCHMODELITEM_H
#define RESEARCHMODELITEM_H

#include <QPixmap>
#include <QString>

namespace Domain {
	class Research;
}


namespace BusinessLogic
{
	/**
	 * @brief Класс элемента модели разработки
	 */
	class ResearchModelItem
	{
	public:
		ResearchModelItem(Domain::Research* _research = 0);
		~ResearchModelItem();

	public:
		/**
		 * @brief Название элемента разработки
		 */
		QString name() const;

		/**
		 * @brief Иконка элемента разработки
		 */
		QPixmap icon() const;

		/**
		 * @brief Получить эелемент разарботки
		 */
		Domain::Research* research() const;

	private:
		/**
		 * @brief Элемент, данные которого будем визуализировать
		 */
		Domain::Research* m_research;

	/**
	 * @brief Вспомогательные методы для организации работы модели
	 */
	/** @{ */
	public:
		/**
		 * @brief Добавить элемент в начало
		 */
		void prependItem(ResearchModelItem* _item);

		/**
		 * @brief Добавить элемент в конец
		 */
		void appendItem(ResearchModelItem* _item);

		/**
		 * @brief Вставить элемент в указанное место
		 */
		void insertItem(int _index, ResearchModelItem* _item);

		/**
		 * @brief Удалить элемент
		 */
		void removeItem(ResearchModelItem* _item);

		/**
		 * @brief Имеет ли элемент родительский элемент
		 */
		bool hasParent() const;

		/**
		 * @brief Родительский элемент
		 */
		ResearchModelItem* parent() const;

		/**
		 * @brief Дочерний элемент по индексу
		 */
		ResearchModelItem* childAt(int _index) const;

		/**
		 * @brief Индекс дочернего элемента
		 */
		int rowOfChild(ResearchModelItem* _child) const;

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
		ResearchModelItem* m_parent;

		/**
		 * @brief Дочерние элементы
		 */
		QList<ResearchModelItem*> m_children;

	/** @} */
	};
}

#endif // RESEARCHMODELITEM_H
