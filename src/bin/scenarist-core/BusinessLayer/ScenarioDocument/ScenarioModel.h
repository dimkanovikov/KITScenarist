#ifndef SCENARIOMODEL_H
#define SCENARIOMODEL_H

#include <BusinessLayer/Counters/Counter.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>


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

		/**
		 * @brief Индексы ролей с данными в модели
		 */
		enum DataRoles {
			ColorIndex = Qt::UserRole + 1,
			TitleIndex,
			SceneTextIndex,
			DescriptionIndex,
			DurationIndex,
			SceneNumberIndex,
			HasNoteIndex,
			VisibilityIndex
		};

	public:
		explicit ScenarioModel(QObject *parent, ScenarioXml* _xmlHandler);
		~ScenarioModel();

		/**
		 * @brief Добавить элемент в начало сценария
		 */
		void prependItem(ScenarioModelItem* _item, ScenarioModelItem* _parentItem = 0);

		/**
		 * @brief Добавить элемент в сценарий
		 * @note Позиция вставки определяется по позиции элемента в тексте
		 */
		void addItem(ScenarioModelItem* _item, ScenarioModelItem* _parentItem = 0);

		/**
		 * @brief Вставить элемент после родственика
		 */
		void insertItem(ScenarioModelItem* _item, ScenarioModelItem* _afterSiblingItem);

		/**
		 * @brief Удалить элемент
		 */
		void removeItem(ScenarioModelItem* _item);

		/**
		 * @brief Обновить элемент
		 */
		void updateItem(ScenarioModelItem* _item);

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
		int duration() const;

		/**
		 * @brief Суммарный счётчик сценария
		 */
		Counter counter() const;

		/**
		 * @brief Получить элемент находящийся в заданном индексе
		 */
		ScenarioModelItem* itemForIndex(const QModelIndex& _index) const;

		/**
		 * @brief Получить индекс заданного элемента
		 */
		QModelIndex indexForItem(ScenarioModelItem* _item) const;

		/**
		 * @brief Получить порядковый номер элемента в заданном индексе
		 */
		int numberForIndex(const QModelIndex& _index) const;

		/**
		 * @brief Получить индекс элемента имеющего заданный порядковый номер
		 */
		QModelIndex indexForNumber(int _number) const;

		/**
		 * @brief Сформировать простую схему для сцен
		 */
		QString simpleScheme() const;

	signals:
		/**
		 * @brief Данные опущены в позиции
		 */
		void mimeDropped(int _atPosition);

	private:
		/**
		 * @brief Корневой элемент дерева
		 */
		ScenarioModelItem* m_rootItem;

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

	/**
	 * @brief Класс фильтрующий модель
	 */
	class ScenarioModelFiltered : public QSortFilterProxyModel
	{
	public:
		ScenarioModelFiltered(QObject* _parent = 0) :
			QSortFilterProxyModel(_parent),
			m_dragDropEnabled(true) {}

		/**
		 * @brief Настройка возможности перетаскивания элементов модели
		 */
		/** @{ */
		void setDragDropEnabled(bool _enabled);
		Qt::ItemFlags flags(const QModelIndex &_index) const;
		/** @} */

	protected:
		/**
		 * @brief Переопределяем фильтр для сокрытия не нужных элементов
		 */
		bool filterAcceptsRow(int _sourceRow, const QModelIndex& _sourceParent) const;

	private:
		/**
		 * @brief Доступна ли возможность перетаскивания элементов
		 */
		bool m_dragDropEnabled;
	};
}

#endif // SCENARIOMODEL_H
