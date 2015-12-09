#include "ScenarioDocument.h"

#include "ScenarioXml.h"
#include "ScenarioTextDocument.h"
#include "ScenarioModel.h"
#include "ScenarioModelItem.h"
#include "ScenarioTemplate.h"
#include "ScenarioTextBlockInfo.h"
#include "ScenarioTextBlockParsers.h"

#include <BusinessLayer/Chronometry/ChronometerFacade.h>
#include <BusinessLayer/Counters/CountersFacade.h>

#include <Domain/Scenario.h>

#include <QCryptographicHash>
#include <QRegularExpression>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>

using namespace BusinessLogic;


namespace {
	/**
	 * @brief Получить хэш текста
	 */
	static QByteArray textMd5Hash(const QString& _text) {
		QCryptographicHash hash(QCryptographicHash::Md5);
		hash.addData(_text.toUtf8());
		return hash.result();
	}
}

QString ScenarioDocument::MIME_TYPE = "application/x-scenarist/scenario";

ScenarioDocument::ScenarioDocument(QObject* _parent) :
	QObject(_parent),
	m_scenario(0),
	m_xmlHandler(new ScenarioXml(this)),
	m_document(new ScenarioTextDocument(this, m_xmlHandler)),
	m_model(new ScenarioModel(this, m_xmlHandler)),
	m_inSceneDescriptionUpdate(false)
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

qreal ScenarioDocument::durationAtPosition(int _position) const
{
	qreal duration = 0;

	if (!m_modelItems.isEmpty()) {
		//
		// Определим сцену, в которой находится курсор
		//
		QMap<int, ScenarioModelItem*>::const_iterator iter = m_modelItems.lowerBound(_position);
		if (iter == m_modelItems.end()
			|| (iter.key() > _position
				&& iter != m_modelItems.begin())) {
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
	return m_model->duration();
}

QString ScenarioDocument::countersInfo() const
{
	const int pageCount = m_document->pageCount();
	return BusinessLogic::CountersFacade::countersInfo(pageCount, m_model->counter());
}

QModelIndex ScenarioDocument::itemIndexAtPosition(int _position) const
{
	ScenarioModelItem* item = itemForPosition(_position, true);
	return m_model->indexForItem(item);
}

int ScenarioDocument::itemStartPosition(const QModelIndex& _index) const
{
	ScenarioModelItem* item = m_model->itemForIndex(_index);
	return item->position();
}

int ScenarioDocument::itemEndPosition(const QModelIndex& _index) const
{
	ScenarioModelItem* item = m_model->itemForIndex(_index);
	return item->endPosition();
}

QString ScenarioDocument::itemHeaderAtPosition(int _position) const
{
	QString header;
	if (ScenarioModelItem* item = itemForPosition(_position, true)) {
		header = item->header();
	}
	return header;
}

QString ScenarioDocument::itemColors(ScenarioModelItem* _item) const
{
	QTextCursor cursor(m_document);
	cursor.setPosition(_item->position());

	QString colors;
	QTextBlockUserData* textBlockData = cursor.block().userData();
	if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
		colors = info->colors();
	}
	return colors;
}

void ScenarioDocument::setItemColorsAtPosition(int _position, const QString& _colors)
{
	if (ScenarioModelItem* item = itemForPosition(_position, true)) {
		//
		// Установить цвет в элемент
		//
		item->setColors(_colors);
		m_model->updateItem(item);

		//
		// Установить цвет в документ
		//
		QTextCursor cursor(m_document);
		cursor.setPosition(item->position());

		QTextBlockUserData* textBlockData = cursor.block().userData();
		ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
		if (info == 0) {
			info = new ScenarioTextBlockInfo;
		}
		info->setColors(_colors);
		cursor.block().setUserData(info);
	}
}

QString ScenarioDocument::itemDescriptionAtPosition(int _position) const
{
	QString description;
	if (ScenarioModelItem* item = itemForPosition(_position, true)) {
		description = itemDescription(item);
	}
	return description;
}

QString ScenarioDocument::itemDescription(ScenarioModelItem* _item) const
{
	QTextCursor cursor(m_document);
	cursor.setPosition(_item->position());

	QString description;
	QTextBlockUserData* textBlockData = cursor.block().userData();
	if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
		description = info->description();
	}
	return description;
}

void ScenarioDocument::setItemDescriptionAtPosition(int _position, const QString& _description)
{
	if (!m_inSceneDescriptionUpdate) {
		m_inSceneDescriptionUpdate = true;

		if (ScenarioModelItem* item = itemForPosition(_position, true)) {
			//
			// Установить описание в элемент
			//
			QTextDocument descriptionDoc;
			descriptionDoc.setHtml(_description);
			const QString descriptionPlainText = descriptionDoc.toPlainText();
			item->setDescription(descriptionPlainText);
			m_model->updateItem(item);

			//
			// Установить описание в документ
			//
			QTextCursor cursor(m_document);
			cursor.setPosition(item->position());

			QTextBlockUserData* textBlockData = cursor.block().userData();
			ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
			if (info == 0) {
				info = new ScenarioTextBlockInfo;
			}
			info->setDescription(_description);
			cursor.block().setUserData(info);

			//
			// Обновить описание внутри текста
			//
			cursor.beginEditBlock();
			ScenarioBlockStyle descriptionBlockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::SceneDescription);
			cursor.movePosition(QTextCursor::NextBlock);
			if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
				cursor.movePosition(QTextCursor::NextBlock);
			}
			//
			// ... затираем старый текст
			//
			ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::forBlock(cursor.block());
			if (currentBlockType == ScenarioBlockStyle::SceneDescription) {
				while (currentBlockType == ScenarioBlockStyle::SceneDescription
					   && !cursor.atEnd()) {
					cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
					currentBlockType = ScenarioBlockStyle::forBlock(cursor.block());
				}
				if (!cursor.atEnd()) {
					cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
				}
				cursor.removeSelectedText();
			} else {
				if (!cursor.atEnd()) {
					cursor.movePosition(QTextCursor::Left);
				}
				cursor.insertBlock(descriptionBlockStyle.blockFormat(), descriptionBlockStyle.charFormat());
			}
			//
			// ... вставляем новый
			//
			if (descriptionPlainText.isEmpty()) {
				cursor.deletePreviousChar();
			} else {
				foreach (const QString& descriptionLine, descriptionPlainText.split("\n")) {
					if (!cursor.block().text().isEmpty()) {
						cursor.insertBlock(descriptionBlockStyle.blockFormat(), descriptionBlockStyle.charFormat());
					}
					cursor.block().setVisible(m_document->outlineMode());
					cursor.insertText(descriptionLine);
				}
			}
			cursor.endEditBlock();
		}

		m_inSceneDescriptionUpdate = false;
	}
}

void ScenarioDocument::load(Domain::Scenario* _scenario)
{
	m_scenario = _scenario;

	if (m_scenario != 0) {
		load(m_scenario->text());
	}
}

Domain::Scenario* ScenarioDocument::scenario() const
{
	return m_scenario;
}

void ScenarioDocument::setScenario(Domain::Scenario* _scenario)
{
	if (m_scenario != _scenario) {
		m_scenario = _scenario;
	}
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

void ScenarioDocument::clear()
{
	QTextCursor cursor(m_document);
	cursor.select(QTextCursor::Document);
	cursor.removeSelectedText();
}

QStringList ScenarioDocument::findCharacters() const
{
	//
	// Найти персонажей во всём тексте
	//
	QSet<QString> characters;
	QTextCursor cursor(document());
	while (!cursor.atEnd()) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::Character) {
			cursor.select(QTextCursor::BlockUnderCursor);
			QString character =
					BusinessLogic::CharacterParser::name(cursor.selectedText().toUpper().trimmed());
			characters.insert(character);
		} else if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
			cursor.select(QTextCursor::BlockUnderCursor);
			QStringList blockCharacters = BusinessLogic::SceneCharactersParser::characters(cursor.selectedText());
			foreach (const QString& characterName, blockCharacters) {
				QString character = BusinessLogic::CharacterParser::name(characterName.toUpper().trimmed());
				characters.insert(character);
			}
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	return characters.toList();
}

QStringList ScenarioDocument::findLocations() const
{
	//
	// Найти локации во всём тексте
	//
	QSet<QString> locations;
	QTextCursor cursor(document());
	while (!cursor.atEnd()) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneHeading) {
			QString location =
					BusinessLogic::SceneHeadingParser::locationName(cursor.block().text().toUpper().trimmed());
			locations.insert(location);
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	return locations.toList();
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
			insertPosition = lastChild->endPosition();
		} else {
			int parentStartPosition = _insertParent->position();
			QTextCursor cursor(m_document);
			cursor.setPosition(parentStartPosition);
			//
			// ... переходим к концу блока заголовка
			//
			cursor.movePosition(QTextCursor::NextBlock);
			//
			// ... переходим к концу описания элемента
			//
			while (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneDescription) {
				cursor.movePosition(QTextCursor::NextBlock);
			}
			//
			// ... возвращаемся в конец последнего блока
			//
			cursor.movePosition(QTextCursor::Left);

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

void ScenarioDocument::aboutContentsChange(int _position, int _charsRemoved, int _charsAdded)
{
	//
	// Прерываем ситуация, когда в редактор помещается документ, но для него уже создана модель
	//
	const QByteArray currentTextMd5Hash = ::textMd5Hash(m_document->toPlainText());
	if (_position == 0 && _charsRemoved == _charsAdded
		&& _charsAdded == m_document->characterCount() && !m_modelItems.isEmpty()) {
		//
		// ... на самом ли деле текст изменился?
		//
		if (currentTextMd5Hash == m_lastTextMd5Hash) {
			return;
		}
	}
	//
	// Сохранить md5 хэш текста документа
	//
	m_lastTextMd5Hash = currentTextMd5Hash;

	//
	// Если были удалены данные
	//
	if (_charsRemoved > 0) {
		//
		// Удаляем элементы начиная с того, который находится под курсором, если курсор в начале
		// строки, или со следующего за курсором, если курсор не в начале строки
		//
		QMap<int, ScenarioModelItem*>::iterator iter = m_modelItems.lowerBound(_position);
		const int charsAddedDelta = _charsAdded - _charsRemoved;
		const int charsRemovedDelta = _charsRemoved - _charsAdded;
		while (iter != m_modelItems.end()
			   && iter.key() >= _position
			   && iter.key() < (_position + _charsRemoved)) {
			//
			// Элемент для удаления
			//
			ScenarioModelItem* itemToDelete = iter.value();

			if (itemToDelete != 0) {
				//
				// Расширяем диапозон последующего построения дерева, для включения в него всех
				// кто был удалён тут по причине не самого оптимального алгоритма
				//
				if (itemToDelete->hasChildren()) {
					const int charsModified = itemToDelete->endPosition() - _position;
					if (_charsAdded < charsModified + charsAddedDelta) {
						_charsAdded = charsModified + charsAddedDelta;
					}
					if (_charsRemoved < charsModified - charsRemovedDelta) {
						_charsRemoved = charsModified - charsRemovedDelta;
					}
				}

				//
				// Удалим элемент из модели
				//
				m_model->removeItem(iter.value());
			}

			//
			// Удалим элемент из кэша
			//
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
	// ... исключаем ситуацию повторного сигнала загрузки документа
	//
	if (_charsAdded != m_document->characterCount()) {
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
			ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::Undefined;
			do {
				cursor.movePosition(QTextCursor::NextBlock);
				cursor.movePosition(QTextCursor::EndOfBlock);
				currentType = ScenarioBlockStyle::forBlock(cursor.block());
			} while (!cursor.atEnd()
					 && currentType != ScenarioBlockStyle::SceneHeading
					 && currentType != ScenarioBlockStyle::SceneGroupHeader
					 && currentType != ScenarioBlockStyle::SceneGroupFooter
					 && currentType != ScenarioBlockStyle::FolderHeader
					 && currentType != ScenarioBlockStyle::FolderFooter);

			//
			// Тип следующего за элементом блока
			//
			ScenarioBlockStyle::Type nextBlockType = ScenarioBlockStyle::Undefined;
			//
			// Если не конец документа, то получить стиль следующего за элементом блока
			// и оступить на один блок назад в виду того, что мы зашли на следующий элемент
			//
			if (!cursor.atEnd()
				|| currentType == ScenarioBlockStyle::SceneHeading
				|| currentType == ScenarioBlockStyle::SceneGroupHeader
				|| currentType == ScenarioBlockStyle::SceneGroupFooter
				|| currentType == ScenarioBlockStyle::FolderHeader
				|| currentType == ScenarioBlockStyle::FolderFooter) {
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
				ScenarioBlockStyle::Type checkType = ScenarioBlockStyle::forBlock(cursorForCheck.block());
				if (checkType != ScenarioBlockStyle::SceneGroupFooter
					&& checkType != ScenarioBlockStyle::FolderFooter) {
					updateItem(currentItem, currentItemStartPos, currentItemEndPos);
					m_model->updateItem(currentItem);
				}
			}

			//
			// Определим родителя если ещё не определён
			//
			if (currentParent == 0) {
				if (currentItem->type() == ScenarioModelItem::Scene
					|| currentItem->type() == ScenarioModelItem::Undefined) {
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
					case ScenarioBlockStyle::SceneHeading: {
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

					case ScenarioBlockStyle::SceneGroupHeader:
					case ScenarioBlockStyle::FolderHeader: {
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

					case ScenarioBlockStyle::SceneGroupFooter:
					case ScenarioBlockStyle::FolderFooter: {
						//
						// Делаем текущим родителем родителя группирующего элемента, чтобы последующие
						// элементы уже не вкладывались, а создавались рядом
						//
						if (currentItem != currentParent) {
							currentItem = currentItem->parent();
						}
						currentParent = currentItem->parent();

						//
						// Сохраняем окончание группирующего блока
						//
						currentItem->setFooter(cursor.block().text());
						break;
					}

					default:
						break;
				}
			}

		} while (!cursor.atEnd()
				 && cursor.position() < (_position + _charsAdded));
	}

	updateDocumentScenesNumbers();
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
	cursor.setPosition(_itemStartPos);
	// ... тип
	ScenarioModelItem::Type itemType = ScenarioModelItem::Undefined;
	ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::forBlock(cursor.block());
	if (blockType == ScenarioBlockStyle::SceneHeading) {
		itemType = ScenarioModelItem::Scene;
	} else if (blockType == ScenarioBlockStyle::SceneGroupHeader
			   || blockType == ScenarioBlockStyle::SceneGroupFooter) {
		itemType = ScenarioModelItem::SceneGroup;
	} else if (blockType == ScenarioBlockStyle::FolderHeader
			   || blockType == ScenarioBlockStyle::FolderFooter) {
		itemType = ScenarioModelItem::Folder;
	}
	// ... заголовок
	QString itemHeader = cursor.block().text();
	// ... цвет
	const QString colors = itemColors(_item);
	// ... текст и описание
	QString itemText;
	QString description;
	cursor.movePosition(QTextCursor::NextBlock);
	while (!cursor.atEnd()
		   && cursor.position() < _itemEndPos) {
		cursor.movePosition(QTextCursor::EndOfBlock);

		ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::forBlock(cursor.block());
		//
		// ... исключаем из текста описание
		//
		if (blockType != ScenarioBlockStyle::SceneDescription) {
			if (!itemText.isEmpty()) {
				itemText.append(" ");
			}
			ScenarioBlockStyle blockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(blockType);
			itemText +=
					blockStyle.charFormat().fontCapitalization() == QFont::AllUppercase
					? cursor.block().text().toUpper()
					: cursor.block().text();
		} else {
			if (!description.isEmpty()) {
				description.append("\n");
			}
			description.append(cursor.block().text());
		}

		cursor.movePosition(QTextCursor::NextBlock);
	}
	//
	// ... обновляем описание в зависимости от способа его обновления
	//
	if (m_inSceneDescriptionUpdate) {
		//
		// ... пользователь изменил описание в окошке
		//
		QTextDocument doc;
		doc.setHtml(itemDescription(_item));
		description = doc.toPlainText().replace("\n", " ");
	} else {
		//
		// ... пользователь изменил описание прямо в редакторе сценария
		//

		//
		// TODO: какое безобразие, нужно это явно сделать красиво!
		//
		QTextCursor descriptionCursor = cursor;
		descriptionCursor.setPosition(_itemStartPos);
		QTextBlockUserData* textBlockData = descriptionCursor.block().userData();
		ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
		if (info == 0) {
			info = new ScenarioTextBlockInfo;
		}
		info->setPlainDescription(description);
		descriptionCursor.block().setUserData(info);

		//
		// ... убираем переносы строк
		//
		description.replace("\n", " ");
	}

	// ... длительность
	qreal itemDuration = 0;
	if (itemType == ScenarioModelItem::Scene) {
		itemDuration = ChronometerFacade::calculate(m_document, _itemStartPos, _itemEndPos);
	}
	// ... содержит ли примечания
	bool hasNote = false;
	cursor.setPosition(_itemStartPos);
	while (cursor.position() < _itemEndPos) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		if (ScenarioBlockStyle::forBlock(cursor.block())
			== ScenarioBlockStyle::NoprintableText) {
			hasNote = true;
			break;
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}
	// ... счётчик слов и символов
	Counter counter = CountersFacade::calculate(m_document, _itemStartPos, _itemEndPos);

	//
	// Обновим данные элемента
	//
	_item->setType(itemType);
	_item->setHeader(itemHeader);
	_item->setColors(colors);
	_item->setText(itemText);
	_item->setDescription(description);
	_item->setDuration(itemDuration);
	_item->setHasNote(hasNote);
	_item->setCounter(counter);
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

void ScenarioDocument::updateDocumentScenesNumbers()
{
	m_model->updateSceneNumbers();

	//
	// Проходим документ и обновляем номера сцен
	//
	QTextBlock block = document()->begin();
	while (block.isValid()) {
		if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
			if (ScenarioModelItem* item = itemForPosition(block.position())) {
				//
				// Обновим данные документа
				//
				QTextBlockUserData* textBlockData = block.userData();
				ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
				if (info == 0) {
					info = new ScenarioTextBlockInfo;
				}
				info->setSceneNumber(item->sceneNumber());
				block.setUserData(info);
			}
		}

		block = block.next();
	}
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
	m_document->load(_scenario);
	int documentCharactersCount = m_document->characterCount();
	aboutContentsChange(0, 0, documentCharactersCount);

	//
	// Подключаем необходимые сигналы
	//
	initConnections();
}
