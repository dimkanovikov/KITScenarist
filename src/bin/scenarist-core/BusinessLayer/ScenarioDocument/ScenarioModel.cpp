#include "ScenarioModel.h"

#include "ScenarioModelItem.h"
#include "ScenarioDocument.h"
#include "ScenarioXml.h"

#include <QMimeData>

using namespace BusinessLogic;


QString ScenarioModel::MIME_TYPE = "application/x-scenarist/scenario-tree";

ScenarioModel::ScenarioModel(QObject *parent, ScenarioXml* _xmlHandler) :
	QAbstractItemModel(parent),
	m_rootItem(new ScenarioModelItem(0)),
	m_xmlHandler(_xmlHandler),
	m_lastMime(0),
	m_scenesCount(0)
{
	Q_ASSERT(m_xmlHandler);

	m_rootItem->setHeader(QObject::tr("Scenario"));
	m_rootItem->setType(ScenarioModelItem::Scenario);
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
		_parentItem = m_rootItem;
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

void ScenarioModel::addItem(ScenarioModelItem* _item, ScenarioModelItem* _parentItem)
{
	//
	// Если родитель не задан им становится сам сценарий
	//
	if (_parentItem == 0) {
		_parentItem = m_rootItem;
	}

	//
	// Если такого элемента ещё нет у родителя
	//
	if (_parentItem->rowOfChild(_item) == -1) {
		QModelIndex parentIndex = indexForItem(_parentItem);

		//
		// Определим позицию вставки
		//
		int itemRowIndex = 0;
		for (int childIndex = _parentItem->childCount() - 1; childIndex >= 0; --childIndex) {
			ScenarioModelItem* child = _parentItem->childAt(childIndex);
			if (child->position() < _item->position()) {
				itemRowIndex = childIndex + 1;
				break;
			}
		}

		beginInsertRows(parentIndex, itemRowIndex, itemRowIndex);
		_parentItem->insertItem(itemRowIndex, _item);
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
	//
	// Спрева удаляем всех детей
	//
	if (_item->hasChildren()) {
		for (int childIndex = _item->childCount()-1; childIndex >= 0; --childIndex) {
			removeItem(_item->childAt(childIndex));
		}
	}

	//
	// Затем удаляем сам элемент
	//
	ScenarioModelItem* itemParent = _item->parent();
	QModelIndex itemParentIndex = indexForItem(_item).parent();
	int itemRowIndex = itemParent->rowOfChild(_item);

	beginRemoveRows(itemParentIndex, itemRowIndex, itemRowIndex);
	itemParent->removeItem(_item);
	endRemoveRows();
}

void ScenarioModel::updateItem(ScenarioModelItem* _item)
{
	//
	// Если элемент уже в списке, то обновим, в противном случае просто игнорируем
	//
	if (_item->parent() != 0) {
		const QModelIndex indexForUpdate = indexForItem(_item);
		emit dataChanged(indexForUpdate, indexForUpdate);
	}
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
		// Цвет сцены
		//
		case ColorIndex: {
			result = item->colors();
			break;
		}

		//
		// Название сцены
		//
		case TitleIndex: {
			result = item->title();
			break;
		}

		//
		// Текст сцены
		//
		case SceneTextIndex: {
			if (item->type() == ScenarioModelItem::Scenario) {
				result = item->description();
			} else {
				result = item->text();
			}
			break;
		}

		//
		// Описание
		//
		case DescriptionIndex: {
			result = item->description();
			break;
		}

		//
		// Длительность
		//
		case DurationIndex: {
			result = item->duration();
			break;
		}

		//
		// Номер сцены
		//
		case SceneNumberIndex: {
			if (item->type() == ScenarioModelItem::Scene) {
				result = item->sceneNumber();
			}
			break;
		}

		//
		// Если ли заметки для данного элемента
		//
		case HasNoteIndex: {
			result = item->hasNote();
			break;
		}

		//
		// Видимость элемента
		//
		case VisibilityIndex: {
			result = true;
			//
			// Скрываем только первый блок, если он содержит текст ИЗ ЗТМ
			//
			if (item->type() == ScenarioModelItem::Undefined && item->header().toUpper() == tr("FADE IN:")) {
				result = false;
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
				bool removeLastMime = false;
				if (m_lastMime == _data) {
					removeLastMime = true;
				}

				//
				// Вставим данные
				//
				int insertPosition = m_xmlHandler->xmlToScenario(parentItem, insertBeforeItem, _data->data(MIME_TYPE), removeLastMime);
				isDropSucceed = true;
				emit mimeDropped(insertPosition);

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
		// Выделение может быть только последовательным, но нужно учесть ситуацию, когда в выделение
		// попадает родительский элемент и не все его дочерние элементы, поэтому просто использовать
		// последний элемент некорректно, нужно проверить, не входит ли его родитель в выделение
		//

		QModelIndexList correctedIndexes;
		foreach (const QModelIndex& index, _indexes) {
			if (!_indexes.contains(index.parent())) {
				correctedIndexes.append(index);
			}
		}

		//
		// Для того, чтобы запретить разрывать папки проверяем выделены ли элементы одного уровня
		//
		bool itemsHaveSameParent = true;
		{
			const QModelIndex& genericParent = correctedIndexes.first().parent();
			foreach (const QModelIndex& index, correctedIndexes) {
				if (index.parent() != genericParent) {
					itemsHaveSameParent = false;
					break;
				}
			}
		}

		//
		// Если выделены элементы одного уровня, то создаём майм-данные
		//
		if (itemsHaveSameParent) {
			qSort(correctedIndexes);

			QModelIndex fromIndex = correctedIndexes.first();
			QModelIndex toIndex = correctedIndexes.last();

			//
			// Определяем элементы из которых будет состоять выделение
			//
			ScenarioModelItem* fromItem = itemForIndex(fromIndex);
			ScenarioModelItem* toItem = itemForIndex(toIndex);

			//
			// Сформируем данные
			//
			mimeData->setData(
						MIME_TYPE,
						m_xmlHandler->scenarioToXml(fromItem, toItem).toUtf8());
		}
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
			_item->setSceneNumber(++_sceneNumberCounter);
		}
		//
		// В противном случае обновим детей элемента
		//
		else {
			for (int itemIndex = 0; itemIndex < _item->childCount(); ++itemIndex) {
				updateSceneNumbers(_item->childAt(itemIndex), _sceneNumberCounter);
			}
		}
	}
}

void ScenarioModel::updateSceneNumbers()
{
	int sceneNumber = 0;
	for (int itemIndex = 0; itemIndex < m_rootItem->childCount(); ++itemIndex) {
		::updateSceneNumbers(m_rootItem->childAt(itemIndex), sceneNumber);
	}

	m_scenesCount = sceneNumber;

	//
	// Сигналим, о том, что модель обновилась
	//
	const QModelIndex rootIndex = indexForItem(m_rootItem);
	emit dataChanged(rootIndex, rootIndex);
}

int ScenarioModel::scenesCount() const
{
	return m_scenesCount;
}

int ScenarioModel::duration() const
{
	return m_rootItem->duration();
}

Counter ScenarioModel::counter() const
{
	return m_rootItem->counter();
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

namespace {
	static int scenarioModelNumberForItem(ScenarioModelItem* _parent, ScenarioModelItem* _item, int& _number) {
		int searchedNumber = -1;
		for (int childIndex = 0; childIndex < _parent->childCount(); ++childIndex) {
			ScenarioModelItem* child = _parent->childAt(childIndex);
			if (child == _item) {
				searchedNumber = _number;
				break;
			} else {
				++_number;
				if (child->hasChildren()) {
					searchedNumber = scenarioModelNumberForItem(child, _item, _number);
					if (searchedNumber != -1) {
						break;
					}
				}
			}
		}

		return searchedNumber;
	}
}

int ScenarioModel::numberForIndex(const QModelIndex& _index) const
{
	int number = 0;
	ScenarioModelItem* item = itemForIndex(_index);
	return ::scenarioModelNumberForItem(m_rootItem, item, number);
}

namespace {
	static ScenarioModelItem* scenarioModelItemForNumber(ScenarioModelItem* _parent, int _number, int& _currentNumber) {
		ScenarioModelItem* searchedItem = nullptr;
		for (int childIndex = 0; childIndex < _parent->childCount(); ++childIndex) {
			ScenarioModelItem* child = _parent->childAt(childIndex);
			if (_currentNumber == _number) {
				searchedItem = child;
				break;
			} else {
				++_currentNumber;
				if (child->hasChildren()) {
					searchedItem = scenarioModelItemForNumber(child, _number, _currentNumber);
					if (searchedItem != nullptr) {
						break;
					}
				}
			}
		}

		return searchedItem;
	}
}

QModelIndex ScenarioModel::indexForNumber(int _number) const
{
	int currentNumber = 0;
	return indexForItem(::scenarioModelItemForNumber(m_rootItem, _number, currentNumber));
}

namespace {
	const int CARD_WIDTH = 210;
	const int CARD_HEIGHT = 100;
	const int CARDS_SPACE = 40;

	/**
	 * @brief Сколько займут вложенные дети
	 */
	static QSize cardChildsSize(const ScenarioModelItem* _parent) {
		int width = CARD_WIDTH + CARDS_SPACE;
		int height = CARD_HEIGHT + CARDS_SPACE;
		for (int childIndex = 0; childIndex < _parent->childCount(); ++childIndex) {
			const ScenarioModelItem* childItem = _parent->childAt(childIndex);
			if (childItem->hasChildren()) {
				QSize childSize = cardChildsSize(childItem);
				width += childSize.width() + CARDS_SPACE;
				if (height < childSize.height() + CARDS_SPACE) {
					height = childSize.height() + CARDS_SPACE;
				}
			} else {
				width += CARD_WIDTH + CARDS_SPACE;
			}
		}

		return QSize(width, height);
	}

	/**
	 * @brief Сформировать xml схемы для детей элемента
	 */
	static QString cardChildsXml(const ScenarioModelItem* _parent, int& _id) {
		QString xml;
		int x = CARDS_SPACE;
		int y = CARDS_SPACE;
		const int parentId = _id;
		++_id;
		for (int childIndex = 0; childIndex < _parent->childCount(); ++childIndex) {
			const ScenarioModelItem* child = _parent->childAt(childIndex);
			if (child->hasChildren()) {
				//
				// Прикинуть размер
				//
				const QSize itemSize = cardChildsSize(child);
				xml.append(QString("<ActionShape "
								   "id=\"%1\" "
								   "parent_id=\"%2\" "
								   "x=\"%3\" "
								   "y=\"%4\" "
								   "width=\"%5\" "
								   "height=\"%6\" "
								   "card_type=\"%7\" "
								   "title=\"%8\" "
								   "description=\"%9\"/>\n")
						   .arg(_id)
						   .arg(parentId)
						   .arg(x)
						   .arg(y)
						   .arg(itemSize.width())
						   .arg(itemSize.height())
						   .arg(child->type())
						   .arg(child->title().isEmpty() ? child->header() : child->title())
						   .arg(child->description()));
				//
				// Строим схему из детей
				//
				xml.append(cardChildsXml(child, _id));

				x += itemSize.width() + CARDS_SPACE;
			} else {
				xml.append(QString("<ActionShape "
								   "id=\"%1\" "
								   "parent_id=\"%2\" "
								   "x=\"%3\" "
								   "y=\"%4\" "
								   "width=\"%5\" "
								   "height=\"%6\" "
								   "card_type=\"%7\" "
								   "title=\"%8\" "
								   "description=\"%9\"/>\n")
						   .arg(_id)
						   .arg(parentId)
						   .arg(x)
						   .arg(y)
						   .arg(CARD_WIDTH)
						   .arg(CARD_HEIGHT)
						   .arg(child->type())
						   .arg(child->title().isEmpty() ? child->header() : child->title())
						   .arg(child->description()));

				x += CARD_WIDTH + CARDS_SPACE;

				++_id;
			}
		}

		return xml;
	}
}

QString ScenarioModel::simpleScheme() const
{
	QString xml("<?xml version=\"1.0\"?>\n"
				"<cards_xml viewx=\"500\" viewy=\"270\">\n");

	//
	// Пробегаем по всем элементам
	// если нет детей, то позиционируем со стандартным размером в заданном смещении
	// если есть дети, то рассчитываем размер, занимаемый детьми и позиционируем всё поддерево
	//

	//
	// Сначала формируем карточки
	//
	int id = 0;
	int x= 60;
	int y = 60;
	for (int childIndex = 0; childIndex < m_rootItem->childCount(); ++childIndex) {
		const ScenarioModelItem* child = m_rootItem->childAt(childIndex);
		if (child->hasChildren()) {
			//
			// Прикинуть размер
			//
			const QSize itemSize = ::cardChildsSize(child);
			xml.append(QString("<ActionShape "
							   "id=\"%1\" "
							   "x=\"%2\" "
							   "y=\"%3\" "
							   "width=\"%4\" "
							   "height=\"%5\" "
							   "card_type=\"%6\" "
							   "title=\"%7\" "
							   "description=\"%8\"/>\n")
					   .arg(id)
					   .arg(x)
					   .arg(y)
					   .arg(itemSize.width())
					   .arg(itemSize.height())
					   .arg(child->type())
					   .arg(child->title().isEmpty() ? child->header() : child->title())
					   .arg(child->description()));
			//
			// Строим схему из детей
			//
			xml.append(::cardChildsXml(child, id));

			x += itemSize.width() + CARDS_SPACE;
		} else {
			xml.append(QString("<ActionShape "
							   "id=\"%1\" "
							   "x=\"%2\" "
							   "y=\"%3\" "
							   "width=\"%4\" "
							   "height=\"%5\" "
							   "card_type=\"%6\" "
							   "title=\"%7\" "
							   "description=\"%8\"/>\n")
					   .arg(id)
					   .arg(x)
					   .arg(y)
					   .arg(CARD_WIDTH)
					   .arg(CARD_HEIGHT)
					   .arg(child->type())
					   .arg(child->title().isEmpty() ? child->header() : child->title())
					   .arg(child->description()));

			x += CARD_WIDTH + CARDS_SPACE;

			++id;
		}
	}

	//
	// А потом соединения между всеми карточками
	//
	int flowEndShapeId = 1;
	const int CARDS_COUNT = id;
	while (flowEndShapeId < CARDS_COUNT) {
		xml.append(QString("<ArrowFlow "
						   "id=\"%1\" "
						   "from_id=\"%2\" "
						   "to_id=\"%3\" "
						   "offsetX=\"0\" offsetY=\"0\" text=\"\" KnotsCount=\"0\"/>\n")
				   .arg(id++)
				   .arg(flowEndShapeId - 1)
				   .arg(flowEndShapeId));
		++flowEndShapeId;
	}
	xml.append("</cards_xml>\n\n");

	return xml;
}

// ********

void ScenarioModelFiltered::setDragDropEnabled(bool _enabled)
{
	if (m_dragDropEnabled != _enabled) {
		m_dragDropEnabled = _enabled;
	}
}

Qt::ItemFlags ScenarioModelFiltered::flags(const QModelIndex &_index) const
{
	Qt::ItemFlags result = sourceModel()->flags(mapToSource(_index));
	if (!m_dragDropEnabled) {
		result ^= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	}
	return result;
}

bool ScenarioModelFiltered::filterAcceptsRow(int _sourceRow, const QModelIndex& _sourceParent) const
{
	QModelIndex index = sourceModel()->index(_sourceRow, 0, _sourceParent);
	bool visible = sourceModel()->data(index, ScenarioModel::VisibilityIndex).toBool();
	return visible;
}
