#include "ScenarioModel.h"

#include "ScenarioModelItem.h"
#include "ScenarioDocument.h"
#include "ScenarioXml.h"

#include <QMimeData>

using namespace BusinessLogic;


QString ScenarioModel::MIME_TYPE = "application/x-scenarist/scenario-tree";

ScenarioModel::ScenarioModel(QObject *parent, ScenarioXml* _xmlHandler) :
	QAbstractItemModel(parent),
	m_rootItem(0),
	m_scenarioItem(new ScenarioModelItem),
	m_xmlHandler(_xmlHandler),
	m_lastMime(0),
	m_scenesCount(0)
{
	Q_ASSERT(m_xmlHandler);

	m_scenarioItem->setHeader(QObject::tr("Scenario"));
	m_scenarioItem->setType(ScenarioModelItem::Scenario);
	m_rootItem = m_scenarioItem;
//	m_rootItem->appendItem(m_scenarioItem);
}

ScenarioModel::~ScenarioModel()
{
	delete m_rootItem;
	m_rootItem = 0;
}

void ScenarioModel::prependItem(ScenarioModelItem* _item, ScenarioModelItem* _parentItem)
{
	//
	// Если родитель не задан им становится сам сценарий
	//
	if (_parentItem == 0) {
		_parentItem = m_scenarioItem;
	}

	//
	// Если такого элемента ещё нет у родителя
	//
	if (_parentItem->rowOfChild(_item) == -1) {
		QModelIndex parentIndex = indexForItem(_parentItem);
		int itemRowIndex = 0; // т.к. в самое начало

		beginInsertRows(parentIndex, itemRowIndex, itemRowIndex);
		_parentItem->prependItem(_item);
		endInsertRows();
	}
}

void ScenarioModel::appendItem(ScenarioModelItem* _item, ScenarioModelItem* _parentItem)
{
	//
	// Если родитель не задан им становится сам сценарий
	//
	if (_parentItem == 0) {
		_parentItem = m_scenarioItem;
	}

	//
	// Если такого элемента ещё нет у родителя
	//
	if (_parentItem->rowOfChild(_item) == -1) {
		QModelIndex parentIndex = indexForItem(_parentItem);
		int itemRowIndex = _parentItem->childCount();

		beginInsertRows(parentIndex, itemRowIndex, itemRowIndex);
		_parentItem->appendItem(_item);
		endInsertRows();
	}
}

void ScenarioModel::insertItem(ScenarioModelItem* _item, ScenarioModelItem* _afterSiblingItem)
{
	ScenarioModelItem* parent = _afterSiblingItem->parent();

	//
	// Если такого элемента ещё нет у родителя
	//
	if (parent->rowOfChild(_item) == -1) {
		QModelIndex parentIndex = indexForItem(parent);
		int itemRowIndex = parent->rowOfChild(_afterSiblingItem) + 1;

		beginInsertRows(parentIndex, itemRowIndex, itemRowIndex);
		parent->insertItem(itemRowIndex, _item);
		endInsertRows();
	}
}

void ScenarioModel::removeItem(ScenarioModelItem* _item)
{
	ScenarioModelItem* itemParent = _item->parent();
	QModelIndex itemParentIndex = indexForItem(_item).parent();
	int itemRowIndex = itemParent->rowOfChild(_item);

	beginRemoveRows(itemParentIndex, itemRowIndex, itemRowIndex);
	itemParent->removeItem(_item);
	endRemoveRows();
}

QModelIndex ScenarioModel::index(int _row, int _column, const QModelIndex& _parent) const
{
	QModelIndex resultIndex;
	if (_row < 0
		|| _row > rowCount(_parent)
		|| _column < 0
		|| _column > columnCount(_parent)
		|| (_parent.isValid() && (_parent.column() != 0))
		) {
		resultIndex = QModelIndex();
	} else {
		ScenarioModelItem* parentItem = itemForIndex(_parent);
		Q_ASSERT(parentItem);

		ScenarioModelItem* indexItem = parentItem->childAt(_row);
		if (indexItem != 0) {
			resultIndex = createIndex(_row, _column, indexItem);
		}
	}
	return resultIndex;
}

QModelIndex ScenarioModel::parent(const QModelIndex& _child) const
{
	QModelIndex parentIndex;
	if (_child.isValid()) {
		ScenarioModelItem* childItem = itemForIndex(_child);
		ScenarioModelItem* parentItem = childItem->parent();
		if (parentItem != 0
			&& parentItem != m_rootItem) {
			ScenarioModelItem* grandParentItem = parentItem->parent();
			if (grandParentItem != 0) {
				int row = grandParentItem->rowOfChild(parentItem);
				parentIndex = createIndex(row, 0, parentItem);
			}
		}
	}
	return parentIndex;
}

int ScenarioModel::columnCount(const QModelIndex&) const
{
	return 1;
}

int ScenarioModel::rowCount(const QModelIndex& _parent) const
{
	int rowCount = 0;
	if (_parent.isValid() && (_parent.column() != 0)) {
		//
		// Ноль строк
		//
	} else {
		ScenarioModelItem* item = itemForIndex(_parent);
		if (item != 0) {
			rowCount = item->childCount();
		}
	}
	return rowCount;
}

Qt::ItemFlags ScenarioModel::flags(const QModelIndex& _index) const
{
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

	ScenarioModelItem* item = itemForIndex(_index);
	if (item->type() == ScenarioModelItem::Folder
		|| item->type() == ScenarioModelItem::SceneGroup
		|| item->type() == ScenarioModelItem::Scenario) {
		flags |= Qt::ItemIsDropEnabled;
	}

	return flags;
}

QVariant ScenarioModel::data(const QModelIndex& _index, int _role) const
{
	QVariant result;

	ScenarioModelItem* item = itemForIndex(_index);
	switch (_role) {
		case Qt::DisplayRole: {
			result = item->header();
			break;
		}

		case Qt::DecorationRole: {
			result = QVariant::fromValue(item->icon());
			break;
		}

		//
		// Текст сцены
		//
		case Qt::UserRole + 1: {
			if (item->type() == ScenarioModelItem::Scenario) {
				result = item->synopsis();
			} else {
				result = item->text();
			}
			break;
		}

		//
		// Синопсис
		//
		case Qt::UserRole + 2: {
			result = item->synopsis();
			break;
		}

		//
		// Длительность
		//
		case Qt::UserRole + 3: {
			result = item->duration();
			break;
		}

		//
		// Номер сцены
		//
		case Qt::UserRole + 4: {
			if (item->type() == ScenarioModelItem::Scene) {
				result = item->number();
			}
			break;
		}

		default: {
			break;
		}
	}

	return result;
}

bool ScenarioModel::dropMimeData(
		const QMimeData* _data, Qt::DropAction _action,
		int _row, int _column, const QModelIndex& _parent)
{
	/*
	 * Вставка данных в этом случае происходит напрямую в текст документа, а не в дерево,
	 * само дерево просто перестраивается после всех манипуляций с текстовым редактором
	 */

	Q_UNUSED(_column);

	//
	// _row - индекс, куда вставлять, если в папку, то он равен -1 и если в самый низ списка, то он тоже равен -1
	//

	bool isDropSucceed = false;

	if (_data != 0
		&& _data->hasFormat(MIME_TYPE)) {

		switch (_action) {
			case Qt::IgnoreAction: {
				isDropSucceed = true;
				break;
			}

			case Qt::MoveAction:
			case Qt::CopyAction: {
				//
				// Получим структурные элементы дерева, чтобы понять, куда вкладывать данные
				//
				// ... элемент, в который будут вкладываться данные
				ScenarioModelItem* parentItem = itemForIndex(_parent);
				// ... элемент, перед которым будут вкладываться данные
				ScenarioModelItem* insertBeforeItem = parentItem->childAt(_row);

				//
				// Если производится перемещение данных
				//
				if (m_lastMime == _data) {
					m_xmlHandler->removeLastMime();
				}

				//
				// Вставим данные
				//
				m_xmlHandler->xmlToScenario(parentItem, insertBeforeItem, _data->data(MIME_TYPE));
				isDropSucceed = true;

				break;
			}

			default: {
				break;
			}
		}
	}

	return isDropSucceed;
}

QMimeData* ScenarioModel::mimeData(const QModelIndexList& _indexes) const
{
	QMimeData* mimeData = new QMimeData;

	if (!_indexes.isEmpty()) {
		//
		// Т.к. выделение может быть только последовательным, то не переживая
		// используем первый и последний индексы выления
		//

		ScenarioModelItem* fromItem = itemForIndex(_indexes.first());
		ScenarioModelItem* toItem = itemForIndex(_indexes.last());

		//
		// Сформируем данные
		//
		mimeData->setData(
					MIME_TYPE,
					m_xmlHandler->scenarioToXml(fromItem, toItem).toUtf8());
	}

	m_lastMime = mimeData;

	return mimeData;
}

QStringList ScenarioModel::mimeTypes() const
{
	return QStringList() << MIME_TYPE;
}

Qt::DropActions ScenarioModel::supportedDragActions() const
{
	return Qt::MoveAction;
}

Qt::DropActions ScenarioModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

namespace {
	void updateSceneNumbers(ScenarioModelItem* _item, int& _sceneNumberCounter) {
		//
		// Если элемент сцена, то обновим номер
		//
		if (_item->type() == ScenarioModelItem::Scene) {
			_item->setNumber(++_sceneNumberCounter);
		}
		//
		// В противном случае обновим детей элемента
		//
		else {
			for (int itemIndex = 0; itemIndex < _item->childCount(); ++itemIndex) {
				::updateSceneNumbers(_item->childAt(itemIndex), _sceneNumberCounter);
			}
		}
	}
}

void ScenarioModel::updateSceneNumbers()
{
	int sceneNumber = 0;
	for (int itemIndex = 0; itemIndex < m_scenarioItem->childCount(); ++itemIndex) {
		::updateSceneNumbers(m_scenarioItem->childAt(itemIndex), sceneNumber);
	}

	m_scenesCount = sceneNumber;
}

int ScenarioModel::scenesCount() const
{
	return m_scenesCount;
}

int ScenarioModel::fullDuration() const
{
	return m_rootItem->duration();
}

ScenarioModelItem* ScenarioModel::itemForIndex(const QModelIndex& _index) const
{
	ScenarioModelItem* resultItem = m_rootItem;
	if (_index.isValid()) {
		ScenarioModelItem* item = static_cast<ScenarioModelItem*>(_index.internalPointer());
		if (item != 0) {
			resultItem = item;
		}
	}
	return resultItem;
}

QModelIndex ScenarioModel::indexForItem(ScenarioModelItem* _item) const
{
	if (_item == 0) {
		return QModelIndex();
	}

	QModelIndex parent;
	if (_item->hasParent()
		&& _item->parent()->hasParent()) {
		parent = indexForItem(_item->parent());
	} else {
		parent = QModelIndex();
	}

	int row;
	if (_item->hasParent()
		&& _item->parent()->hasParent()) {
		row = _item->parent()->rowOfChild(_item);
	} else {
		row = m_rootItem->rowOfChild(_item);
	}

	return index(row, 0, parent);
}
