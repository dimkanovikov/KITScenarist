#include "ScenarioXml.h"

#include "ScenarioDocument.h"
#include "ScenarioTextDocument.h"
#include "ScenarioModelItem.h"
#include "ScenarioTextBlockStyle.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextCursor>
#include <QXmlStreamReader>

using namespace BusinessLogic;


ScenarioXml::ScenarioXml(ScenarioDocument* _scenario) :
	m_scenario(_scenario),
	m_lastMimeFrom(0),
	m_lastMimeTo(0)
{
	Q_ASSERT(m_scenario);
}

QString ScenarioXml::scenarioToXml(int _startPosition, int _endPosition)
{
	QString resultXml;

	//
	// Если необходимо обработать весь текст
	//
	if (_startPosition == 0
		&& _endPosition == 0) {
		_endPosition = m_scenario->document()->characterCount();
	}

	//
	// Сохраним позиции
	//
	m_lastMimeFrom = _startPosition;
	m_lastMimeTo = _endPosition;

	//
	// Получим курсор для редактирования
	//
	QTextCursor cursor(m_scenario->document());

	//
	// Переместимся к началу текста для формирования xml
	//
	cursor.setPosition(_startPosition);

	//
	// Подсчитаем кол-во незакрытых групп и папок, и закроем, если необходимо
	//
	int openedGroups = 0;
	int openedFolders = 0;

	do {
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

		ScenarioTextBlockStyle::Type currentType = ScenarioTextBlockStyle::forBlock(cursor.block());

		//
		// Курсор в конце текущего блока
		// или в конце выделения
		//
		if (cursor.atBlockEnd()
			|| cursor.position() == _endPosition) {
			//
			// Получить текст под курсором
			//
			QString textToSave =
					QString("<![CDATA[%1]]>")
					.arg(cursor.selectedText().simplified());


			//
			// Дописать xml
			//
			switch (currentType) {
				case ScenarioTextBlockStyle::TimeAndPlace: {
					resultXml += "<time_and_place>";
					resultXml += textToSave;
					resultXml += "</time_and_place>";
					break;
				}

				case ScenarioTextBlockStyle::Action: {
					resultXml += "<action>";
					resultXml += textToSave;
					resultXml += "</action>";
					break;
				}

				case ScenarioTextBlockStyle::Character: {
					resultXml += "<character>";
					resultXml += textToSave;
					resultXml += "</character>";
					break;
				}

				case ScenarioTextBlockStyle::Parenthetical: {
					if (textToSave.startsWith("(")) {
						textToSave = textToSave.mid(1);
					}
					if (textToSave.endsWith(")")) {
						textToSave = textToSave.left(textToSave.length()-1);
					}

					if (!textToSave.isEmpty()) {
						resultXml += "<parenthetical>";
						resultXml += textToSave;
						resultXml += "</parenthetical>";
					}
					break;
				}

				case ScenarioTextBlockStyle::Dialog: {
					resultXml += "<dialog>";
					resultXml += textToSave;
					resultXml += "</dialog>";
					break;
				}

				case ScenarioTextBlockStyle::Transition:{
					resultXml += "<transition>";
					resultXml += textToSave;
					resultXml += "</transition>";
					break;
				}

				case ScenarioTextBlockStyle::Note: {
					resultXml += "<note>";
					resultXml += textToSave;
					resultXml += "</note>";
					break;
				}

				case ScenarioTextBlockStyle::Title: {
					resultXml += "<title>";
					resultXml += textToSave;
					resultXml += "</title>";
					break;
				}

				case ScenarioTextBlockStyle::NoprintableText: {
					resultXml += "<noprintable_text>";
					resultXml += textToSave;
					resultXml += "</noprintable_text>";
					break;
				}

				case ScenarioTextBlockStyle::SceneGroupHeader: {
					resultXml += "<scene_group>";
					resultXml += "<scene_group_header>";
					resultXml += textToSave;
					resultXml += "</scene_group_header>";

					++openedGroups;

					break;
				}

				case ScenarioTextBlockStyle::SceneGroupFooter: {
					//
					// Закрываем группы, если были открыты, то просто корректируем счётчик,
					// а если открытых нет, то не записываем и конец
					//
					if (openedGroups > 0) {
						--openedGroups;

						resultXml += "<scene_group_footer>";
						resultXml += textToSave;
						resultXml += "</scene_group_footer>";
						resultXml += "</scene_group>";
					}
					break;
				}

				case ScenarioTextBlockStyle::FolderHeader: {
					resultXml += "<folder>";
					resultXml += "<folder_header>";
					resultXml += textToSave;
					resultXml += "</folder_header>";

					++openedFolders;

					break;
				}

				case ScenarioTextBlockStyle::FolderFooter: {
					//
					// Закрываем папки, если были открыты, то просто корректируем счётчик,
					// а если открытых нет, то не записываем и конец
					//
					if (openedFolders > 0) {
						--openedFolders;

						resultXml += "<folder_footer>";
						resultXml += textToSave;
						resultXml += "</folder_footer>";
						resultXml += "</folder>";
					}
					break;
				}

				default: {
					break;
				}
			}

			//
			// Снимем выделение
			//
			cursor.clearSelection();
		}

	} while (cursor.position() < _endPosition
			 && !cursor.atEnd());

	//
	// Закроем открытые группы
	//
	while (openedGroups > 0) {
		resultXml += "<scene_group_footer>";
		resultXml += QObject::tr("END OF GROUP", "ScenarioXml");
		resultXml += "</scene_group_footer>";
		resultXml += "</scene_group>";
		--openedGroups;
	}

	//
	// Закроем открытые папки
	//
	while (openedFolders > 0) {
		resultXml += "<folder_footer>";
		resultXml += QObject::tr("END OF FOLDER", "ScenarioXml");
		resultXml += "</folder_footer>";
		resultXml += "</folder>";
		--openedFolders;
	}

	//
	// Добавим корневой элемент
	//
	resultXml.prepend("<scenario>");
	resultXml.append("</scenario>");

	return resultXml.simplified();
}

QString ScenarioXml::scenarioToXml(ScenarioModelItem* _fromItem, ScenarioModelItem* _toItem)
{
	//
	// Определить интервал текста из которого нужно создать xml-представление
	//
	// ... начало
	int startPosition = m_scenario->itemStartPosition(_fromItem);
	// ... конец
	int endPosition = m_scenario->itemEndPosition(_fromItem);

	int toItemEndPosition = m_scenario->itemEndPosition(_toItem);
	if (endPosition < toItemEndPosition) {
		endPosition = toItemEndPosition;
	}

	//
	// Сформировать xml-строку
	//
	return scenarioToXml(startPosition, endPosition);
}

void ScenarioXml::xmlToScenario(int _position, const QString& _xml)
{
	//
	// Происходит ли обработка первого блока
	//
	bool firstBlockHandling = true;
	//
	// Необходимо ли изменить тип блока, в который вставляется текст
	//
	bool needChangeFirstBlockType = false;

	//
	// Начинаем операцию вставки
	//
	QTextCursor cursor(m_scenario->document());
	cursor.beginEditBlock();
	cursor.setPosition(_position);

	//
	// Если вставка в пустой блок, то изменим его тип
	//
	if (cursor.block().text().simplified().isEmpty()) {
		needChangeFirstBlockType = true;
	}

	QXmlStreamReader reader(_xml);
	while (!reader.atEnd()) {
		switch (reader.readNext()) {
			case QXmlStreamReader::StartElement: {
				//
				// Определить тип текущего блока
				//
				ScenarioTextBlockStyle::Type tokenType = ScenarioTextBlockStyle::Undefined;
				QString tokenName = reader.name().toString();
				if (tokenName == "time_and_place") {
					tokenType = ScenarioTextBlockStyle::TimeAndPlace;
				} else if (tokenName == "action") {
					tokenType = ScenarioTextBlockStyle::Action;
				} else if (tokenName == "character") {
					tokenType = ScenarioTextBlockStyle::Character;
				} else if (tokenName == "parenthetical") {
					tokenType = ScenarioTextBlockStyle::Parenthetical;
				} else if (tokenName == "dialog") {
					tokenType = ScenarioTextBlockStyle::Dialog;
				} else if (tokenName == "transition") {
					tokenType = ScenarioTextBlockStyle::Transition;
				} else if (tokenName == "note") {
					tokenType = ScenarioTextBlockStyle::Note;
				} else if (tokenName == "title") {
					tokenType = ScenarioTextBlockStyle::Title;
				} else if (tokenName == "noprintable_text") {
					tokenType = ScenarioTextBlockStyle::NoprintableText;
				} else if (tokenName == "scene_group_header") {
					tokenType = ScenarioTextBlockStyle::SceneGroupHeader;
				} else if (tokenName == "scene_group_footer") {
					tokenType = ScenarioTextBlockStyle::SceneGroupFooter;
				} else if (tokenName == "folder_header") {
					tokenType = ScenarioTextBlockStyle::FolderHeader;
				} else if (tokenName == "folder_footer") {
					tokenType = ScenarioTextBlockStyle::FolderFooter;
				}

				//
				// Если определён тип блока, то обработать его
				//
				if (tokenType != ScenarioTextBlockStyle::Undefined) {
					ScenarioTextBlockStyle currentStyle(tokenType);

					if (!firstBlockHandling) {
						cursor.insertBlock();
					}

					//
					// Если нужно добавим заголовок стиля
					//
					if (currentStyle.hasHeader()) {
						ScenarioTextBlockStyle headerStyle(currentStyle.headerType());
						cursor.setBlockCharFormat(headerStyle.charFormat());
						cursor.setBlockFormat(headerStyle.blockFormat());
						cursor.insertText(currentStyle.header());
						cursor.insertBlock();
					}

					//
					// Если необходимо сменить тип блока
					//
					if ((firstBlockHandling && needChangeFirstBlockType)
						|| !firstBlockHandling) {

						//
						// Установим стиль блока
						//
						cursor.setBlockCharFormat(currentStyle.charFormat());
						cursor.setBlockFormat(currentStyle.blockFormat());
					}

					//
					// Корректируем информацию о шаге
					//
					if (firstBlockHandling) {
						firstBlockHandling = false;
					}
				}

				break;
			}

			case QXmlStreamReader::Characters: {
				cursor.insertText(reader.text().toString().simplified());
				break;
			}

			default: {
				break;
			}
		}
	}

	//
	// Завершаем операцию
	//
	cursor.endEditBlock();
}

void ScenarioXml::xmlToScenario(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore, const QString& _xml, bool _removeLastMime)
{
	//
	// Начинаем операцию вставки
	//
	QTextCursor cursor(m_scenario->document());
	cursor.beginEditBlock();

	//
	// Определим позицию для вставки данных
	//
	int insertPosition = m_scenario->positionToInsertMime(_insertParent, _insertBefore);

	//
	// Если необходимо удалить прошлое выделение
	//
	if (_removeLastMime) {
		bool needCorrectPosition = false;
		if (m_lastMimeFrom < insertPosition) {
			needCorrectPosition = true;
		}

		int removedSymbols = removeLastMime();
		if (needCorrectPosition) {
			insertPosition -= removedSymbols;
		}
	}

	//
	// Вставим пустой блок для нового элемента
	//
	cursor.setPosition(insertPosition);
	cursor.insertBlock();
	//
	// ... скорректируем позицию курсора
	//
	if (insertPosition != 0) {
		insertPosition = cursor.position();
	}

	//
	// Вставка данных
	//
	xmlToScenario(insertPosition, _xml);

	//
	// Завершаем операцию
	//
	cursor.endEditBlock();
}

int ScenarioXml::removeLastMime()
{
	int removedSymbols = 0;

	if (m_lastMimeFrom != m_lastMimeTo
		&& m_lastMimeFrom < m_lastMimeTo) {
		//
		// Расширим область чтобы не оставалось пустых строк
		//
		if (m_lastMimeFrom > 0) {
			--m_lastMimeFrom;
		} else if (m_lastMimeTo != (m_scenario->document()->characterCount() - 1)){
			++m_lastMimeTo;
		}

		QTextCursor cursor(m_scenario->document());
		cursor.setPosition(m_lastMimeFrom);
		cursor.setPosition(m_lastMimeTo, QTextCursor::KeepAnchor);
		cursor.removeSelectedText();

		removedSymbols = m_lastMimeTo - m_lastMimeFrom;
	}

	m_lastMimeFrom = 0;
	m_lastMimeTo = 0;

	return removedSymbols;
}
