#include "NavigatorItemsModel.h"

#include "NavigatorItem.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioXml.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>
#include <BusinessLogic/ScenarioTextEdit/MimeData/MimeDataProcessor.h>

#include <QXmlStreamReader>
#include <QScrollBar>
#include <QMimeData>


NavigatorItemsModel::NavigatorItemsModel(QObject* _parent, ScenarioTextEdit* _editor) :
	QAbstractItemModel(_parent),
	m_editor(_editor),
	m_rootItem(new NavigatorItem),
	m_dropDeleteFrom(-1),
	m_dropDeleteTo(-1),
	m_canUpdateStructure(true)
{
	//
	// Редактор обязательно должен быть задан
	//
	Q_ASSERT(_editor);

	initConnections();
}

NavigatorItemsModel::~NavigatorItemsModel()
{
	delete m_rootItem;
	m_rootItem = 0;
}

QModelIndex NavigatorItemsModel::index(int _row, int _column, const QModelIndex& _parent) const
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
		NavigatorItem* parentItem = itemForIndex(_parent);
		Q_ASSERT(parentItem);

		NavigatorItem* indexItem = parentItem->childAt(_row);
		if (indexItem != 0) {
			resultIndex = createIndex(_row, _column, indexItem);
		}
	}
	return resultIndex;
}

QModelIndex NavigatorItemsModel::parent(const QModelIndex& _child) const
{
	QModelIndex parentIndex;
	if (_child.isValid()) {
		NavigatorItem* childItem = itemForIndex(_child);
		NavigatorItem* parentItem = childItem->parent();
		if (parentItem != 0
			&& parentItem != m_rootItem) {
			NavigatorItem* grandParentItem = parentItem->parent();
			if (grandParentItem != 0) {
				int row = grandParentItem->rowOfChild(parentItem);
				parentIndex = createIndex(row, 0, parentItem);
			}
		}
	}
	return parentIndex;
}

int NavigatorItemsModel::columnCount(const QModelIndex&) const
{
	return 1;
}

int NavigatorItemsModel::rowCount(const QModelIndex& _parent) const
{
	int rowCount = 0;
	if (_parent.isValid() && (_parent.column() != 0)) {
		//
		// Ноль строк
		//
	} else {
		NavigatorItem* item = itemForIndex(_parent);
		if (item != 0) {
			rowCount = item->childCount();
		}
	}
	return rowCount;
}

Qt::ItemFlags NavigatorItemsModel::flags(const QModelIndex& _index) const
{
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

	NavigatorItem* item = itemForIndex(_index);
	if (item->isFolder()
		|| item == m_rootItem) {
		flags |= Qt::ItemIsDropEnabled;
	}

	return flags;
}

QVariant NavigatorItemsModel::data(const QModelIndex& _index, int _role) const
{
	QVariant result;

	NavigatorItem* item = itemForIndex(_index);
	switch (_role) {
		case Qt::DisplayRole: {
			result = item->header();
			break;
		}

		case Qt::DecorationRole: {
			result = QVariant::fromValue(item->icon());
			break;
		}

		case Qt::ToolTipRole: {
			result = item->description();
			break;
		}

		case Qt::UserRole: {
			result = item->timing();
			break;
		}

		case Qt::UserRole + 1: {
			return item->headerBlock().blockFormat().property(ScenarioTextBlockStyle::PropertyID);
			break;
		}

		default: {
			break;
		}
	}

	return result;
}

bool NavigatorItemsModel::dropMimeData(const QMimeData* _data, Qt::DropAction _action, int _row, int _column, const QModelIndex& _parent)
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

	//
	// На время операции вставки данных запрещаем обновлять структуру
	//
	m_canUpdateStructure = false;

	if (_data != 0
		&& _data->hasFormat(MimeDataProcessor::SCENARIO_MIME_TYPE)) {

		switch (_action) {
			case Qt::IgnoreAction: {
				isDropSucceed = true;
				break;
			}

			case Qt::MoveAction:
			case Qt::CopyAction: {

				//
				// Если текст будет вставляться ПЕРЕД каким-либо блоком, то
				// необходимо отступить назад на один символ и вставлять туда
				//
				bool needStepBack = false;

				//
				// Получим структурные элементы дерева, чтобы понять, куда вкладывать данные
				//
				// ... элемент, в который будут вкладываться данные
				NavigatorItem* parentItem = itemForIndex(_parent);
				// ... элемент, перед которым будут вкладываться данные
				NavigatorItem* childItem = parentItem->childAt(_row);

				//
				// Определим позицию в тексте документа, куда необходимо вставить данные
				//
				int insertPosition = 0;

				//
				// Если вкладывание происходит перед каким-либо структурным элементом
				//
				if (childItem != 0) {
					insertPosition = childItem->headerBlock().position();
					needStepBack = true;
				}
				//
				// Если вкладывание происходит в папку, то нужно вставить текст перед блоком закрывающим её
				//
				else if (parentItem->isFolder()
						 && parentItem != m_rootItem) {
					insertPosition = parentItem->endBlock().position();
					needStepBack = true;
				}
				//
				// В оставшихся случаях вкладываем данные за последним блоком
				//
				else {
					insertPosition = parentItem->endBlock().position() + parentItem->endBlock().length() - 1;
				}

				//
				// Установим курсор в позицию для вставки
				//
				QTextCursor cursor(m_editor->document());
				cursor.setPosition(insertPosition);

				//
				// Если это перемещение текста из самого навигатора, то перед вставкой его нужно удалить.
				// Позиции были запомнены на шаге создания майм-данных при выделении элементов в дереве.
				//
				if (m_dropDeleteFrom != -1 && m_dropDeleteTo != -1) {
					QTextCursor deleteCursor(m_editor->document());
					deleteCursor.setPosition(m_dropDeleteFrom);
					deleteCursor.setPosition(m_dropDeleteTo, QTextCursor::KeepAnchor);
					deleteCursor.deleteChar();
					if (deleteCursor.position() == 0) {
						deleteCursor.deleteChar();
					} else {
						deleteCursor.deletePreviousChar();
					}
				}

				//
				// Подготовим редактор ко вставке текста
				//
				cursor.insertBlock();
				if (needStepBack) {
					cursor.movePosition(QTextCursor::Left);
				}
				cursor.setBlockFormat(QTextBlockFormat());
				m_editor->setTextCursor(cursor);

				//
				// Вставить текст перемещаемого объекта
				//
				MimeDataProcessor::insertFromMime(m_editor, _data);

				isDropSucceed = true;

				break;
			}

			default: {
				break;
			}
		}
	}

	m_canUpdateStructure = true;

	if (isDropSucceed) {
		aboutUpdateStructure();
	}

	return isDropSucceed;
}

QMimeData* NavigatorItemsModel::mimeData(const QModelIndexList& _indexes) const
{
	QMimeData* mimeData = new QMimeData;

	if (!_indexes.isEmpty()) {
		//
		// Т.к. выделение может быть только последовательным, то не переживая
		// используем первый и последний индексы выления
		//

		NavigatorItem* fromItem = itemForIndex(_indexes.first());
		NavigatorItem* toItem = itemForIndex(_indexes.last());

		QTextBlock fromBlock =
				fromItem->headerBlock().blockNumber() < toItem->headerBlock().blockNumber()
				? fromItem->headerBlock()
				: toItem->headerBlock();
		QTextBlock toBlock =
				fromItem->endBlock().blockNumber() > toItem->endBlock().blockNumber()
				? fromItem->endBlock()
				: toItem->endBlock();

		mimeData->setData(
					MimeDataProcessor::SCENARIO_MIME_TYPE,
					MimeDataProcessor::createMimeFromBlocks(m_editor, fromBlock, toBlock).toUtf8());

		m_dropDeleteFrom = fromBlock.position();
		m_dropDeleteTo = toBlock.position() + toBlock.length() - 1;
	}

	return mimeData;
}

QStringList NavigatorItemsModel::mimeTypes() const
{
	return QStringList() << MimeDataProcessor::SCENARIO_MIME_TYPE;
}

Qt::DropActions NavigatorItemsModel::supportedDragActions() const
{
	return Qt::MoveAction;
}

Qt::DropActions NavigatorItemsModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

void NavigatorItemsModel::aboutscrollEditorToItem(const QModelIndex& _index)
{
	NavigatorItem* item = itemForIndex(_index);
	QTextCursor cursor = m_editor->textCursor();
	cursor.setPosition(item->headerBlock().position());
	m_editor->verticalScrollBar()->setValue(m_editor->verticalScrollBar()->maximum());
	m_editor->setTextCursor(cursor);
	m_editor->setFocus();
}

void NavigatorItemsModel::aboutUpdateStructure()
{
	if (m_editor->document()->isEmpty()
		|| !m_canUpdateStructure) {
		return;
	}

	m_dropDeleteFrom = -1;
	m_dropDeleteTo = -1;

	/*
	 * Полное перестроение структуры не очень эффективно, особенно в больших проектах,
	 * но зато очень упрощает понамание, и мне пока не удалось придумать способ кэширования,
	 * который бы значительно повысил эффективность и не сильно усложнил код
	 */
	emit beginUpdateStructure();
	beginResetModel();

	//
	// Пересоздать дерево элементов
	//
	delete m_rootItem;
	m_rootItem = new NavigatorItem;

	//
	// Построить дерево при помощи курсора
	//
	QTextCursor cursor(m_editor->document());
	cursor.movePosition(QTextCursor::Start);

	//
	// Сохранить первый и последний блоки для корня
	//
	m_rootItem->setHeaderBlock(cursor.block());
	cursor.movePosition(QTextCursor::End);
	m_rootItem->setEndBlock(cursor.block());
	cursor.movePosition(QTextCursor::Start);

	// Родительский элемент текущего места в сценарии
	NavigatorItem* currentParent = m_rootItem;
	// Текущий элемент сценария
	NavigatorItem* currentChild = 0;

	while (!cursor.atEnd()) {
		cursor.movePosition(QTextCursor::EndOfBlock);

		QTextBlock currentTextBlock = cursor.block();
		ScenarioTextBlockStyle::Type currentType = m_editor->scenarioBlockType(currentTextBlock);

		switch (currentType) {
			case ScenarioTextBlockStyle::TimeAndPlace: {
				//
				// Закрыть предыдущую сцену
				//
				if (currentChild != 0
					&& !currentChild->isFolder()) {
					currentChild->setEndBlock(currentTextBlock.previous());
				}

				//
				// Создать структурный элемент сцены
				//
				currentChild = new NavigatorItem;
				currentChild->setHeaderBlock(currentTextBlock);
				currentParent->append(currentChild);
				break;
			}

			case ScenarioTextBlockStyle::FolderHeader: {
				//
				// Закрыть предыдущую сцену
				//
				if (currentChild != 0
					&& !currentChild->isFolder()) {
					currentChild->setEndBlock(currentTextBlock.previous());
				}

				//
				// Создать структурный элемент папки
				//
				currentChild = new NavigatorItem;
				currentChild->setHeaderBlock(currentTextBlock);
				currentParent->append(currentChild);

				//
				// Все последующие элементы вкладываются в папку
				//
				currentParent = currentChild;
				break;
			}

			case ScenarioTextBlockStyle::FolderFooter: {
				//
				// Закрыть необходимые элементы
				//
				if (currentChild != 0
					&& !currentChild->isFolder()) {
					currentChild->setEndBlock(currentTextBlock.previous());
				}
				currentParent->setEndBlock(currentTextBlock);

				//
				// Устанавливаем папку последним текущим, а родителем возвращаем родителя папки
				//
				currentChild = currentParent;
				currentParent = currentParent->parent();
				break;
			}

			default: {
				break;
			}
		}

		cursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Закрыть последнюю сцену
	//
	if (currentChild != 0
		&& !currentChild->isFolder()) {
		currentChild->setEndBlock(cursor.block());
	}

	//
	// Завершить обновление модели
	//
	endResetModel();
	emit endUpdateStructure();
}

void NavigatorItemsModel::initConnections()
{
	connect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateStructure()));
}

NavigatorItem* NavigatorItemsModel::itemForIndex(const QModelIndex& _index) const
{
	NavigatorItem* resultItem = m_rootItem;
	if (_index.isValid()) {
		NavigatorItem* item = static_cast<NavigatorItem*>(_index.internalPointer());
		if (item != 0) {
			resultItem = item;
		}
	}
	return resultItem;
}
