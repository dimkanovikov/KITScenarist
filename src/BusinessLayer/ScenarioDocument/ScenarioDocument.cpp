#include "ScenarioDocument.h"
#include "ScenarioModel.h"
#include "ScenarioModelItem.h"
#include "ScenarioTextBlockStyle.h"

#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>

using namespace BusinessLogic;


QString ScenarioDocument::MIME_TYPE = "application/x-scenarius/scenario";

ScenarioDocument::ScenarioDocument(QObject* _parent) :
	QObject(_parent),
	m_document(new QTextDocument(this)),
	m_model(new ScenarioModel(this))
{
	initConnections();
}

QTextDocument* ScenarioDocument::document() const
{
	return m_document;
}

QAbstractItemModel* ScenarioDocument::model() const
{
	return m_model;
}
#include <QDebug>
void ScenarioDocument::aboutContentsChange(int _position, int _charsRemoved, int _charsAdded)
{
	QTextCursor test(m_document);
	test.setPosition(_position+_charsAdded - 1); // Установить курсор в тот блок, в котором закончилось редактирование
	test.movePosition(QTextCursor::EndOfBlock);
	qDebug() << _position << _charsRemoved << _charsAdded << m_document->characterCount() << test.position();

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
			   && iter.key() > _position
			   && iter.key() < (_position + _charsRemoved)) {
			m_model->removeItem(iter.value());
			iter = m_modelItems.erase(iter);
		}
	}

	//
	// Сместить позиции всех сохранённых в кэше элементов после текущего на _charsRemoved и _charsAdded
	//
	{
		QMutableMapIterator<int, ScenarioModelItem*> removeIter(m_modelItems);
		QMap<int, ScenarioModelItem*> updatedItems;
		//
		// Изымаем элементы из хэша и формируем обновлённый список
		//
		while (removeIter.hasNext()) {
			removeIter.next();
			if (removeIter.key() > _position) {
				updatedItems.insert(removeIter.key() - _charsRemoved + _charsAdded, removeIter.value());
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
			currentItem = new ScenarioModelItem;
			m_model->appendItem(currentItem);
			m_modelItems.insert(0, currentItem);
		}
		//
		// Или если вставляется новый элемент в начале текста
		//
		else if (_position == 0 && iter == m_modelItems.begin() && iter.key() > 0) {
			currentItem = new ScenarioModelItem;
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

		ScenarioModelItem* currentParent = 0;
		if (currentItem->type() == ScenarioModelItem::Scene) {
			currentParent = currentItem->parent();
		} else {
			currentParent = currentItem;
		}

		QTextCursor cursor(m_document);
		cursor.setPosition(currentItemStartPos);

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
						ScenarioModelItem* newItem = new ScenarioModelItem;
						//
						// Вставить в группирующий элемент
						//
						if (currentItem == currentParent) {
							m_model->appendItem(newItem, currentParent);
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
						ScenarioModelItem* newItem = new ScenarioModelItem;
						//
						// Вставить в группирующий элемент
						//
						if (currentItem == currentParent) {
							m_model->appendItem(newItem, currentParent);
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
	cursor.movePosition(QTextCursor::EndOfBlock);
	cursor.setPosition(_itemEndPos, QTextCursor::KeepAnchor);
	QString itemText = cursor.selectedText().simplified();
	// ... длительность
	int itemDuration = 0;
	if (itemType == ScenarioModelItem::Scene) {
		itemDuration = ChronometerFacade::calculate(m_document, _itemStartPos, _itemEndPos);
	}

	//
	// Обновим данные элемента
	//
	_item->setHeader(itemHeader);
	_item->setType(itemType);
	_item->setText(itemText);
	_item->setDuration(itemDuration);
}
