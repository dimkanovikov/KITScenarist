#include "ScenarioDocument.h"

#include "ScenarioXml.h"
#include "ScenarioTextDocument.h"
#include "ScenarioModel.h"
#include "ScenarioModelItem.h"
#include "ScenarioTextBlockStyle.h"
#include "ScenarioTextBlockInfo.h"

#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <Domain/Scenario.h>

#include <QRegularExpression>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>

using namespace BusinessLogic;


QString ScenarioDocument::MIME_TYPE = "application/x-scenarist/scenario";

ScenarioDocument::ScenarioDocument(QObject* _parent) :
	QObject(_parent),
	m_xmlHandler(new ScenarioXml(this)),
	m_document(new ScenarioTextDocument(this, m_xmlHandler)),
	m_model(new ScenarioModel(this, m_xmlHandler))
{
	initConnections();
}

ScenarioTextDocument* ScenarioDocument::document() const
{
	return m_document;
}

ScenarioModel* ScenarioDocument::model() const
{
	return m_model;
}

int ScenarioDocument::scenesCount() const
{
	return m_model->scenesCount();
}

int ScenarioDocument::durationAtPosition(int _position) const
{
	int duration = 0;

	if (!m_modelItems.isEmpty()) {
		//
		// Определим сцену, в которой находится курсор
		//
		QMap<int, ScenarioModelItem*>::const_iterator iter = m_modelItems.lowerBound(_position);
		if (iter.key() > _position) {
			--iter;
		}

		//
		// Запомним позицию начала сцены
		//
		int startPositionInLastScene = iter.key();

		//
		// Посчитаем хронометраж всех предыдущих сцен
		//
		if (iter.value()->type() == ScenarioModelItem::Scene) {
			iter.value()->duration();
		}
		while (iter != m_modelItems.begin()) {
			--iter;
			if (iter.value()->type() == ScenarioModelItem::Scene) {
				duration += iter.value()->duration();
			}
		}

		//
		// Добавим к суммарному хрономертажу хронометраж от начала сцены
		//
		duration += ChronometerFacade::calculate(m_document, startPositionInLastScene, _position);
	}

	return duration;
}

int ScenarioDocument::fullDuration() const
{
	return m_model->fullDuration();
}

QModelIndex ScenarioDocument::itemIndexAtPosition(int _position) const
{
	ScenarioModelItem* item = itemForPosition(_position, true);
	return m_model->indexForItem(item);
}

int ScenarioDocument::itemStartPosition(const QModelIndex& _index) const
{
	ScenarioModelItem* item = m_model->itemForIndex(_index);
	return itemStartPosition(item);
}

int ScenarioDocument::itemEndPosition(const QModelIndex& _index) const
{
	ScenarioModelItem* item = m_model->itemForIndex(_index);
	return itemEndPosition(item);
}

QString ScenarioDocument::itemHeaderAtPosition(int _position) const
{
	QString header;
	if (ScenarioModelItem* item = itemForPosition(_position, true)) {
		header = item->header();
	}
	return header;
}

QString ScenarioDocument::itemSynopsisAtPosition(int _position) const
{
	QString synopsis;
	if (ScenarioModelItem* item = itemForPosition(_position, true)) {
		synopsis = itemSynopsis(item);
	}
	return synopsis;
}

QString ScenarioDocument::itemSynopsis(ScenarioModelItem* _item) const
{
	QTextCursor cursor(m_document);
	cursor.setPosition(_item->position());

	QString synopsis;
	QTextBlockUserData* textBlockData = cursor.block().userData();
	if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
		synopsis = info->synopsis();
	}
	return synopsis;
}

void ScenarioDocument::setItemSynopsisAtPosition(int _position, const QString& _synopsis)
{
	if (ScenarioModelItem* item = itemForPosition(_position, true)) {
		QTextCursor cursor(m_document);
		cursor.setPosition(item->position());

		QTextBlockUserData* textBlockData = cursor.block().userData();
		ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
		if (info == 0) {
			info = new ScenarioTextBlockInfo;
		}
		info->setSynopsis(_synopsis);
		cursor.block().setUserData(info);
	}
}

namespace {
	const QString EMPTY_HTML_PARAGRAPH =
			"<p style=\"-qt-paragraph-type:empty;margin-top:0px;margin-bottom:0px;margin-left:0px;"
			"margin-right:0px;-qt-block-indent:0;text-indent:0px;\"><br/></p>";
}

QString ScenarioDocument::builSynopsisFromScenes() const
{
	QString synopsis;

	//
	// Строим синопсис
	//
	foreach (ScenarioModelItem* item, m_modelItems.values()) {
		QString curItemSynopsis = itemSynopsis(item);
		synopsis.append(curItemSynopsis);

		//
		// Добавляем разделяющую строку
		//
		if (!curItemSynopsis.isEmpty()) {
			synopsis.append(EMPTY_HTML_PARAGRAPH);
		}
	}

	return synopsis;
}

void ScenarioDocument::load(const QString& _scenario)
{
	//
	// Отключаем всё от документа
	//
	removeConnections();

	//
	// Очищаем модель и документ
	//
	{
		int documentCharactersCount = m_document->characterCount();
		aboutContentsChange(0, documentCharactersCount, 0);
		m_document->clear();
	}

	//
	// Загружаем сценарий
	//
	if (!_scenario.isEmpty()) {
		m_xmlHandler->xmlToScenario(0, _scenario);
		int documentCharactersCount = m_document->characterCount();
		aboutContentsChange(0, 0, documentCharactersCount);
	}

	//
	// Подключаем необходимые сигналы
	//
	initConnections();
}

QString ScenarioDocument::save() const
{
	return m_xmlHandler->scenarioToXml();
}

void ScenarioDocument::refresh()
{
	QString scenario = save();
	load(scenario);
}

int ScenarioDocument::positionToInsertMime(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore) const
{
	int insertPosition = 0;

	//
	// Если необходимо вставить перед заданным элементом
	//
	if (_insertBefore != 0) {
		int insertBeforeItemStartPos = m_modelItems.key(_insertBefore);

		//
		// Шаг назад
		//
		insertPosition = insertBeforeItemStartPos - 1;
	}
	//
	// Если необходимо вставить в конец родительского элемента
	//
	else {
		if (_insertParent->hasChildren()) {
			ScenarioModelItem* lastChild = _insertParent->childAt(_insertParent->childCount() - 1);
			insertPosition = itemEndPosition(lastChild);
		} else {
			int parentStartPosition = itemStartPosition(_insertParent);
			QTextCursor cursor(m_document);
			cursor.setPosition(parentStartPosition);
			cursor.movePosition(QTextCursor::EndOfBlock);
			insertPosition = cursor.position();
		}
	}

	//
	// Если необходимо вставить в начало документа
	//
	if (insertPosition < 0) {
		insertPosition = 0;
	}

	return insertPosition;
}

int ScenarioDocument::itemStartPosition(ScenarioModelItem* _item) const
{
	return m_modelItems.key(_item, 0);
}

int ScenarioDocument::itemEndPosition(ScenarioModelItem* _item) const
{
	int endPosition = 0;

	//
	// Для сцены просто идём до следующего начального/конечного блока
	//
	if (_item->type() == ScenarioModelItem::Scene) {
		QTextCursor cursor(m_document);
		cursor.setPosition(m_modelItems.key(_item));

		//
		// От следующего за началом элемента блока
		//
		cursor.movePosition(QTextCursor::EndOfBlock);
		cursor.movePosition(QTextCursor::NextBlock);

		//
		// Пока не дошли до сигнального блока
		//
		ScenarioTextBlockStyle::Type currentType = ScenarioTextBlockStyle::forBlock(cursor.block());
		while (currentType != ScenarioTextBlockStyle::TimeAndPlace
			   && currentType != ScenarioTextBlockStyle::SceneGroupHeader
			   && currentType != ScenarioTextBlockStyle::SceneGroupFooter
			   && currentType != ScenarioTextBlockStyle::FolderHeader
			   && currentType != ScenarioTextBlockStyle::FolderFooter
			   && !cursor.atEnd()) {
			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.movePosition(QTextCursor::NextBlock);
			currentType = ScenarioTextBlockStyle::forBlock(cursor.block());
		}

		//
		// Если не конец документа, то сместимся на один символ назад,
		// т.к. мы перешли к следующему блоку
		//
		if (!cursor.atEnd()) {
			cursor.movePosition(QTextCursor::Left);
		}

		//
		// Это и будет позиция конца элемента
		//
		endPosition = cursor.position();
	}
	//
	// Для остальных идём до закрывающего элемента
	//
	else {
		QTextCursor cursor(m_document);
		cursor.setPosition(m_modelItems.key(_item));

		//
		// Сохраним стиль блока для последующего поиска парного элемента
		//
		ScenarioTextBlockStyle blockStyle(ScenarioTextBlockStyle::forBlock(cursor.block()));

		//
		// От следующего за началом элемента блока
		//
		cursor.movePosition(QTextCursor::NextBlock);
		cursor.movePosition(QTextCursor::EndOfBlock);

		//
		// Счётчик открытых групп на пути к закрывающему элементу
		//
		int openedItems = 0;
		//
		// Пока не конец документа
		//
		ScenarioTextBlockStyle::Type currentType = ScenarioTextBlockStyle::forBlock(cursor.block());
		while (!cursor.atEnd()) {
			//
			// Если встретился открывающий блок, увеличим счётчик
			//
			if (currentType == blockStyle.blockType()) {
				++openedItems;
			}
			//
			// Если встретился закрывающий блок, уменьшим счётчик
			//
			if (currentType == blockStyle.embeddableFooter()) {
				//
				// Если нет открытых групп, то этот блок является завершающим
				//
				if (openedItems == 0) {
					break;
				}
				--openedItems;
			}

			cursor.movePosition(QTextCursor::NextBlock);
			cursor.movePosition(QTextCursor::EndOfBlock);
			currentType = ScenarioTextBlockStyle::forBlock(cursor.block());
		}

		//
		// Это и будет позиция конца элемента
		//
		endPosition = cursor.position();
	}

	return endPosition;
}

void ScenarioDocument::aboutContentsChange(int _position, int _charsRemoved, int _charsAdded)
{
	//
	// Если были удалены данные
	//
	if (_charsRemoved > 0) {
		//
		// Удаляем элементы начиная с того, который находится под курсором, если курсор в начале
		// строки, или со следующего за курсором, если курсор не в начале строки
		//
		QMap<int, ScenarioModelItem*>::iterator iter = m_modelItems.lowerBound(_position);
		while (iter != m_modelItems.end()
			   && iter.key() >= _position
			   && iter.key() < (_position + _charsRemoved)) {
			//
			// Элемент для удаления
			//
			ScenarioModelItem* itemToDelete = iter.value();

			//
			// Если удаляется элемент содержащий потомков, необходимо вынести потомков на уровень выше
			//
			if (itemToDelete->hasChildren()) {
				for (int childIndex = itemToDelete->childCount()-1; childIndex >= 0; --childIndex) {
					m_model->insertItem(itemToDelete->childAt(childIndex), itemToDelete);
				}
			}

			//
			// Удалим элемент из кэша
			//
			m_model->removeItem(iter.value());
			iter = m_modelItems.erase(iter);
		}
	}

	//
	// Скорректируем позицию
	//
	if (_charsRemoved > _charsAdded
		&& _position > 0) {
		++_position;
	}

	//
	// Сместить позиции всех сохранённых в кэше элементов после текущего на _charsRemoved и _charsAdded
	//
	{
		int position = _position;

		//
		// Если нажат энтер, то нужно сместить все элементы, включая тот, перед блоком
		// которого он нажат
		//
		if (_charsAdded == 1 && _charsRemoved == 0) {
			if (m_document->characterAt(_position) == QChar(QChar::ParagraphSeparator)) {
				--position;
			}
		}

		QMutableMapIterator<int, ScenarioModelItem*> removeIter(m_modelItems);
		QMap<int, ScenarioModelItem*> updatedItems;
		//
		// Изымаем элементы из хэша и формируем обновлённый список
		//
		while (removeIter.hasNext()) {
			removeIter.next();
			if (removeIter.key() > position) {
				ScenarioModelItem* item = removeIter.value();
				item->setPosition(removeIter.key() - _charsRemoved + _charsAdded);
				updatedItems.insert(item->position(), item);
				removeIter.remove();
			}
		}
		//
		// Переносим элементы из обновлённого списка в хэш
		//
		QMapIterator<int, ScenarioModelItem*> updateIter(updatedItems);
		while (updateIter.hasNext()) {
			updateIter.next();
			m_modelItems.insert(updateIter.key(), updateIter.value());
		}
	}

	//
	// Если были добавлены данные
	//
	if (_charsAdded > 0) {
		//
		// получить первый блок и обновить/создать его
		// идти по документу, до конца вставленных символов и добавлять блоки
		//
		QMap<int, ScenarioModelItem*>::iterator iter = m_modelItems.lowerBound(_position);
		if (iter != m_modelItems.begin()
			&& iter.key() > _position) {
			--iter;
		}

		//
		// Обновляем структуру
		//

		ScenarioModelItem* currentItem = 0;
		int currentItemStartPos = 0;

		//
		// Если в документе нет ни одного элемента, создадим первый
		//
		if (iter == m_modelItems.end()) {
			currentItem = itemForPosition(0);
			m_model->addItem(currentItem);
			m_modelItems.insert(0, currentItem);
		}
		//
		// Или если вставляется новый элемент в начале текста
		//
		else if (_position == 0 && iter == m_modelItems.begin() && iter.key() > 0) {
			currentItem = itemForPosition(0);
			m_model->prependItem(currentItem);
			m_modelItems.insert(0, currentItem);
		}
		//
		// В противном случае получим необходимый к обновлению элемент
		//
		else {
			currentItem = iter.value();
			currentItemStartPos = iter.key();
		}

		//
		// Текущий родитель
		//
		ScenarioModelItem* currentParent = 0;

		QTextCursor cursor(m_document);
		if (currentItemStartPos > 0) {
			cursor.setPosition(currentItemStartPos);
		}

		do {

			//
			// Идём до конца элемента
			//
			ScenarioTextBlockStyle::Type currentType = ScenarioTextBlockStyle::Undefined;
			do {
				cursor.movePosition(QTextCursor::NextBlock);
				cursor.movePosition(QTextCursor::EndOfBlock);
				currentType = ScenarioTextBlockStyle::forBlock(cursor.block());
			} while (!cursor.atEnd()
					 && currentType != ScenarioTextBlockStyle::TimeAndPlace
					 && currentType != ScenarioTextBlockStyle::SceneGroupHeader
					 && currentType != ScenarioTextBlockStyle::SceneGroupFooter
					 && currentType != ScenarioTextBlockStyle::FolderHeader
					 && currentType != ScenarioTextBlockStyle::FolderFooter);

			//
			// Тип следующего за элементом блока
			//
			ScenarioTextBlockStyle::Type nextBlockType = ScenarioTextBlockStyle::Undefined;
			//
			// Если не конец документа, то получить стиль следующего за элементом блока
			// и оступить на один блок назад в виду того, что мы зашли на следующий элемент
			//
			if (!cursor.atEnd()
				|| currentType == ScenarioTextBlockStyle::TimeAndPlace
				|| currentType == ScenarioTextBlockStyle::SceneGroupHeader
				|| currentType == ScenarioTextBlockStyle::SceneGroupFooter
				|| currentType == ScenarioTextBlockStyle::FolderHeader
				|| currentType == ScenarioTextBlockStyle::FolderFooter) {
				nextBlockType = currentType;
				cursor.movePosition(QTextCursor::PreviousBlock);
				cursor.movePosition(QTextCursor::EndOfBlock);

				//
				// Если курсор вышел за начало текущего блока, то отменим предыдущее действие
				// это может случиться когда обрабатывается последний блок текста,
				// который является заголовочным
				//
				if (cursor.position() < currentItemStartPos) {
					cursor.movePosition(QTextCursor::NextBlock);
					cursor.movePosition(QTextCursor::EndOfBlock);
				}
			}

			int currentItemEndPos = cursor.position();

			//
			// Сформируем элемент, если это не конец группы
			//
			{
				QTextCursor cursorForCheck(m_document);
				cursorForCheck.setPosition(currentItemStartPos);
				ScenarioTextBlockStyle::Type checkType = ScenarioTextBlockStyle::forBlock(cursorForCheck.block());
				if (checkType != ScenarioTextBlockStyle::SceneGroupFooter
					&& checkType != ScenarioTextBlockStyle::FolderFooter) {
					updateItem(currentItem, currentItemStartPos, currentItemEndPos);
				}
			}

			//
			// Определим родителя если ещё не определён
			//
			if (currentParent == 0) {
				if (currentItem->type() == ScenarioModelItem::Scene) {
					currentParent = currentItem->parent();
				} else {
					currentParent = currentItem;
				}
			}

			cursor.movePosition(QTextCursor::NextBlock);
			//
			// Если не конец документа и всё ещё можно строить структуру
			//
			if (!cursor.atEnd()
				&& cursor.position() < (_position + _charsAdded)) {
				//
				// Обновим позицию начала следующего элемента
				//
				currentItemStartPos = cursor.position();

				//
				// Действуем в зависимости от последующего за текущим элементом блока
				//
				switch (nextBlockType) {
					case ScenarioTextBlockStyle::TimeAndPlace: {
						//
						// Создать новый элемент
						//
						ScenarioModelItem* newItem = itemForPosition(cursor.position());
						//
						// Вставить в группирующий элемент
						//
						if (currentItem == currentParent) {
							m_model->addItem(newItem, currentParent);
						}
						//
						// Вставить после текущего элемента
						//
						else {
							m_model->insertItem(newItem, currentItem);
						}
						//
						// Сделать новый элемент текущим
						//
						currentItem = newItem;

						//
						// Сохраним новый элемент в кэше
						//
						m_modelItems.insert(currentItemStartPos, currentItem);
						break;
					}

					case ScenarioTextBlockStyle::SceneGroupHeader:
					case ScenarioTextBlockStyle::FolderHeader: {
						//
						// Создать новый элемент
						//
						ScenarioModelItem* newItem = itemForPosition(cursor.position());
						//
						// Вставить в группирующий элемент
						//
						if (currentItem == currentParent) {
							m_model->addItem(newItem, currentParent);
						}
						//
						// Вставить после текущего элемента
						//
						else {
							m_model->insertItem(newItem, currentItem);
						}
						//
						// Сделать новый элемент текущим
						//
						currentItem = newItem;
						//
						// Сделать текущего родителя собой, т.к. последующие элементы должны вкладываться внутрь
						//
						currentParent = newItem;

						//
						// Сохраним новый элемент в кэше
						//
						m_modelItems.insert(currentItemStartPos, currentItem);
						break;
					}

					case ScenarioTextBlockStyle::SceneGroupFooter:
					case ScenarioTextBlockStyle::FolderFooter: {
						//
						// Делаем текущим родителем родителя группирующего элемента, чтобы последующие
						// элементы уже не вкладывались, а создавались рядом
						//
						if (currentItem != currentParent) {
							currentItem = currentItem->parent();
						}
						currentParent = currentItem->parent();
						break;
					}

					default:
						break;
				}
			}

		} while (!cursor.atEnd()
				 && cursor.position() < (_position + _charsAdded));
	}

	m_model->updateSceneNumbers();

	//
	// TODO: Обновить модель
	//
	m_model->dataChanged(m_model->index(0,0, QModelIndex()), m_model->index(1000, 1000, QModelIndex()));
}

void ScenarioDocument::initConnections()
{
	connect(m_document, SIGNAL(contentsChange(int,int,int)),
			this, SLOT(aboutContentsChange(int,int,int)));
}

void ScenarioDocument::removeConnections()
{
	disconnect(m_document, SIGNAL(contentsChange(int,int,int)),
			   this, SLOT(aboutContentsChange(int,int,int)));
}

void ScenarioDocument::updateItem(ScenarioModelItem* _item, int _itemStartPos, int _itemEndPos)
{
	//
	// Получим данные элемента
	//
	QTextCursor cursor(m_document);
	// ... заголовок
	cursor.setPosition(_itemStartPos);
	QString itemHeader = cursor.block().text().simplified();
	// ... тип
	ScenarioModelItem::Type itemType = ScenarioModelItem::Undefined;
	ScenarioTextBlockStyle::Type blockType = ScenarioTextBlockStyle::forBlock(cursor.block());
	if (blockType == ScenarioTextBlockStyle::TimeAndPlace) {
		itemType = ScenarioModelItem::Scene;
	} else if (blockType == ScenarioTextBlockStyle::SceneGroupHeader
			   || blockType == ScenarioTextBlockStyle::SceneGroupFooter) {
		itemType = ScenarioModelItem::SceneGroup;
	} else if (blockType == ScenarioTextBlockStyle::FolderHeader
			   || blockType == ScenarioTextBlockStyle::FolderFooter) {
		itemType = ScenarioModelItem::Folder;
	}
	// ... текст
	cursor.movePosition(QTextCursor::NextBlock);
	cursor.setPosition(_itemEndPos, QTextCursor::KeepAnchor);
	QString itemText = cursor.selectedText().simplified();
	// ... длительность
	int itemDuration = 0;
	if (itemType == ScenarioModelItem::Scene) {
		itemDuration = ChronometerFacade::calculate(m_document, _itemStartPos, _itemEndPos);
	}
	// ... содержит ли примечания
	bool hasNote = false;
	cursor.setPosition(_itemStartPos);
	while (cursor.position() < _itemEndPos) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		if (ScenarioTextBlockStyle::forBlock(cursor.block())
			== ScenarioTextBlockStyle::NoprintableText) {
			hasNote = true;
			break;
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Обновим данные элемента
	//
	_item->setHeader(itemHeader);
	_item->setType(itemType);
	_item->setText(itemText);
	_item->setDuration(itemDuration);
	_item->setHasNote(hasNote);
}

ScenarioModelItem* ScenarioDocument::itemForPosition(int _position, bool _findNear) const
{
	ScenarioModelItem* item = m_modelItems.value(_position, 0);
	if (item == 0) {
		//
		// Если необходимо ищем ближайшего
		//
		if (_findNear) {
			QMap<int, ScenarioModelItem*>::const_iterator i = m_modelItems.lowerBound(_position);
			if (i != m_modelItems.begin()
				|| i != m_modelItems.end()) {
				if (i != m_modelItems.begin()) {
					--i;
				}
				item = i.value();
			} else {
				//
				// не найден, т.к. в модели нет элементов
				//
			}
		}
		//
		// В противном случае создаём новый элемент
		//
		else {
			item = new ScenarioModelItem(_position);
		}
	}
	return item;
}
