#ifndef RESEARCHMODEL_H
#define RESEARCHMODEL_H

#include <QAbstractItemModel>

namespace Domain {
	class ResearchTable;
}

namespace BusinessLogic
{
	class ResearchModelItem;


	/**
	 * @brief Древовидная модель разработки
	 */
	class ResearchModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		/**
		 * @brief Майм-тип данных дерева сценария
		 */
		static QString MIME_TYPE;

	public:
		explicit ResearchModel(QObject* _parent);
		~ResearchModel();

		/**
		 * @brief Загрузить данные разработки в модель
		 */
		void load(Domain::ResearchTable* _data);

		/**
		 * @brief Очистить все загруженные данные
		 */
		void clear();

		/**
		 * @brief Добавить элемент в начало сценария
		 */
		void prependItem(ResearchModelItem* _item, ResearchModelItem* _parentItem = 0);

		/**
		 * @brief Добавить элемент в сценарий
		 * @note Позиция вставки определяется по позиции элемента в тексте
		 */
		void addItem(ResearchModelItem* _item, ResearchModelItem* _parentItem = 0);

		/**
		 * @brief Вставить элемент после родственика
		 */
		void insertItem(ResearchModelItem* _item, ResearchModelItem* _afterSiblingItem);

		/**
		 * @brief Удалить элемент
		 */
		void removeItem(ResearchModelItem* _item);

		/**
		 * @brief Обновить элемент
		 */
		void updateItem(ResearchModelItem* _item);

		/**
		 * @brief Реализация древовидной модели
		 */
		/** @{ */
		QModelIndex index(int _row, int _column, const QModelIndex &_parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &_child) const;
		int columnCount( const QModelIndex & ) const;
		int rowCount(const QModelIndex &_parent) const;
		Qt::ItemFlags flags(const QModelIndex &_index) const;
		QVariant data(const QModelIndex &_index, int _role ) const;
		//! Реализация перетаскивания элементов
//		bool dropMimeData(const QMimeData* _data, Qt::DropAction _action, int _row, int _column, const QModelIndex& _parent);
//		QMimeData* mimeData(const QModelIndexList& _indexes) const;
//		QStringList mimeTypes() const;
//		Qt::DropActions supportedDragActions() const;
//		Qt::DropActions supportedDropActions() const;
		/** @} */

		/**
		 * @brief Получить элемент находящийся в заданном индексе
		 */
		ResearchModelItem* itemForIndex(const QModelIndex& _index) const;

		/**
		 * @brief Получить индекс заданного элемента
		 */
		QModelIndex indexForItem(ResearchModelItem* _item) const;

	private:
		/**
		 * @brief Корневой элемент дерева
		 */
		ResearchModelItem* m_rootItem;

		/**
		 * @brief Корневой элемент, для ветви разработки
		 */
		ResearchModelItem* m_researchRoot;
	};
}

#endif // RESEARCHMODEL_H
