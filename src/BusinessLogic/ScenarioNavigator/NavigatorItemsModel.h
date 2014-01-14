#ifndef NAVIGATORITEMSMODEL_H
#define NAVIGATORITEMSMODEL_H

#include <QAbstractItemModel>

class NavigatorItem;
class ScenarioTextEdit;


/**
 * @brief Модель элементов навигатора
 */
class NavigatorItemsModel : public QAbstractItemModel
{
	Q_OBJECT

signals:
	void beginUpdateStructure();
	void endUpdateStructure();

public:
	NavigatorItemsModel(QObject* _parent, ScenarioTextEdit* _editor);
	~NavigatorItemsModel();

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
	 * @brief Получить индекс элемента в дереве, в котором установлен курсор редактора
	 */
	QModelIndex indexOfItemUnderCursor() const;

public slots:
	/**
	 * @brief Прокрутить окно редактора до выделенного элемента
	 */
	void aboutscrollEditorToItem(const QModelIndex& _index);

private slots:
	/**
	 * @brief Обновить текущую сцену
	 */
	void aboutUpdateCurrentItem();

	/**
	 * @brief Перестроить структуру полностью
	 */
	void aboutUpdateStructure();

private:
	/**
	 * @brief Настройка соединений для обработки событий
	 */
	void initConnections();

	/**
	 * @brief Получить элемент находящийся в заданном индексе
	 */
	NavigatorItem* itemForIndex(const QModelIndex& _index) const;

	/**
	 * @brief Получить элемент находящийся под курсором
	 */
	NavigatorItem* itemForTextCursor() const;

	/**
	 * @brief Получить индекс элемента
	 */
	QModelIndex indexForItem(NavigatorItem* _item) const;

private:
	/**
	 * @brief Редактор сценария
	 */
	ScenarioTextEdit* m_editor;

	/**
	 * @brief Корневой элемент дерева
	 */
	NavigatorItem* m_rootItem;

	mutable int m_dropDeleteFrom, m_dropDeleteTo;
	bool m_canUpdateStructure;
};

#endif // NAVIGATORITEMSMODEL_H
