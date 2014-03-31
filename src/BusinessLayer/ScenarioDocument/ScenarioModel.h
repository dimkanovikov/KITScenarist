#ifndef SCENARIOMODEL_H
#define SCENARIOMODEL_H

#include <QAbstractItemModel>


namespace BusinessLogic
{
	class ScenarioModelItem;
	class ScenarioXml;


	/**
	 * @brief Древовидная модель сценария
	 */
	class ScenarioModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		/**
		 * @brief Майм-тип данных дерева сценария
		 */
		static QString MIME_TYPE;

	public:
		explicit ScenarioModel(QObject *parent, ScenarioXml* _xmlHandler);
		~ScenarioModel();

		/**
		 * @brief Добавить элемент в начало сценария
		 */
		void prependItem(ScenarioModelItem* _item, ScenarioModelItem* _parentItem = 0);

		/**
		 * @brief Добавить элемент в конец сценария
		 */
		void appendItem(ScenarioModelItem* _item, ScenarioModelItem* _parentItem = 0);

		/**
		 * @brief Вставить элемент после родственика
		 */
		void insertItem(ScenarioModelItem* _item, ScenarioModelItem* _afterSiblingItem);

		/**
		 * @brief Удалить элемент
		 */
		void removeItem(ScenarioModelItem* _item);

		/**
		 * @brief Реализация древовидной модели
		 */
		/** @{ */
		QModelIndex index(int _row, int _column, const QModelIndex &_parent ) const;
		QModelIndex parent(const QModelIndex &_child) const;
		int columnCount( const QModelIndex & ) const;
		int rowCount(const QModelIndex &_parent) const;
		Qt::ItemFlags flags(const QModelIndex &_index) const;
		QVariant data(const QModelIndex &_index, int _role ) const;
		//! Реализация перетаскивания элементов
		bool dropMimeData(const QMimeData* _data, Qt::DropAction _action, int _row, int _column, const QModelIndex& _parent);
		QMimeData* mimeData(const QModelIndexList& _indexes) const;
		QStringList mimeTypes() const;
		Qt::DropActions supportedDragActions() const;
		Qt::DropActions supportedDropActions() const;
		/** @} */

		/**
		 * @brief Обновить номера сцен
		 */
		void updateSceneNumbers();

		/**
		 * @brief Количество сцен в сценарии
		 */
		int scenesCount() const;

		/**
		 * @brief Суммарная длительность сценария
		 */
		int fullDuration() const;

		/**
		 * @brief Получить элемент находящийся в заданном индексе
		 */
		ScenarioModelItem* itemForIndex(const QModelIndex& _index) const;

		/**
		 * @brief Получить индекс заданного элемента
		 */
		QModelIndex indexForItem(ScenarioModelItem* _item) const;

	private:
		/**
		 * @brief Корневой элемент дерева, то же самое, что и m_scenarioItem
		 *
		 * TODO: избавиться от одного из них
		 */
		ScenarioModelItem* m_rootItem;

		/**
		 * @brief Обязательный элемент сценария
		 */
		ScenarioModelItem* m_scenarioItem;

		/**
		 * @brief Обработчик xml
		 */
		ScenarioXml* m_xmlHandler;

		/**
		 * @brief Указатель на последний созданный майм-объект
		 *
		 * Используется для контроля вставки из другого приложения
		 */
		mutable QMimeData* m_lastMime;

		/**
		 * @brief Счётчик количества сцен
		 */
		int m_scenesCount;
	};
}

#endif // SCENARIOMODEL_H
