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


//QString ScenarioXmlWriter::scenarioToXml(QTextDocument* _scenario, int _startPosition, int _endPosition)
//{
//	QString resultXml;

//	//
//	// Если необходимо обработать весь текст
//	//
//	if (_startPosition == 0
//		&& _endPosition == 0) {
//		_endPosition = _scenario->characterCount();
//	}

//	//
//	// Получим курсор для редактирования
//	//
//	QTextCursor cursor(_scenario);

//	//
//	// Переместимся к началу текста для формирования xml
//	//
//	cursor.setPosition(_startPosition);

//	//
//	// Подсчитаем кол-во незакрытых групп и папок, и закроем, если необходимо
//	//
//	int openedGroups = 0;
//	int openedFolders = 0;

//	do {
//		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

//		ScenarioTextBlockStyle::Type currentType = ScenarioTextBlockStyle::forBlock(cursor.block());

//		//
//		// Курсор в конце текущего блока
//		// или в конце выделения
//		//
//		if (cursor.atBlockEnd()
//			|| cursor.position() == _endPosition) {
//			//
//			// Получить текст под курсором
//			//
//			QString textToSave = cursor.selectedText().simplified();

//			//
//			// Дописать xml
//			//
//			switch (currentType) {
//				case ScenarioTextBlockStyle::TimeAndPlace: {
//					resultXml += "<time_and_place>";
//					resultXml += textToSave;
//					resultXml += "</time_and_place>";
//					break;
//				}

//				case ScenarioTextBlockStyle::Action: {
//					resultXml += "<action>";
//					resultXml += textToSave;
//					resultXml += "</action>";
//					break;
//				}

//				case ScenarioTextBlockStyle::Character: {
//					resultXml += "<character>";
//					resultXml += textToSave;
//					resultXml += "</character>";
//					break;
//				}

//				case ScenarioTextBlockStyle::Parenthetical: {
//					if (textToSave.startsWith("(")) {
//						textToSave = textToSave.mid(1);
//					}
//					if (textToSave.endsWith(")")) {
//						textToSave = textToSave.left(textToSave.length()-1);
//					}

//					if (!textToSave.isEmpty()) {
//						resultXml += "<parenthetical>";
//						resultXml += textToSave;
//						resultXml += "</parenthetical>";
//					}
//					break;
//				}

//				case ScenarioTextBlockStyle::Dialog: {
//					resultXml += "<dialog>";
//					resultXml += textToSave;
//					resultXml += "</dialog>";
//					break;
//				}

//				case ScenarioTextBlockStyle::Transition:{
//					resultXml += "<transition>";
//					resultXml += textToSave;
//					resultXml += "</transition>";
//					break;
//				}

//				case ScenarioTextBlockStyle::Note: {
//					resultXml += "<note>";
//					resultXml += textToSave;
//					resultXml += "</note>";
//					break;
//				}

//				case ScenarioTextBlockStyle::Title: {
//					resultXml += "<title>";
//					resultXml += textToSave;
//					resultXml += "</title>";
//					break;
//				}

//				case ScenarioTextBlockStyle::NoprintableText: {
//					resultXml += "<simple_text>";
//					resultXml += textToSave;
//					resultXml += "</simple_text>";
//					break;
//				}

//				case ScenarioTextBlockStyle::SceneGroupHeader: {
//					resultXml += "<scene_group>";
//					resultXml += "<scene_group_header>";
//					resultXml += textToSave;
//					resultXml += "</scene_group_header>";

//					++openedGroups;

//					break;
//				}

//				case ScenarioTextBlockStyle::SceneGroupFooter: {
//					//
//					// Закрываем группы, если были открыты, то просто корректируем счётчик,
//					// а если открытых нет, то не записываем и конец
//					//
//					if (openedGroups > 0) {
//						--openedGroups;

//						resultXml += "<scene_group_footer>";
//						resultXml += textToSave;
//						resultXml += "</scene_group_footer>";
//						resultXml += "</scene_group>";
//					}
//					break;
//				}

//				case ScenarioTextBlockStyle::FolderHeader: {
//					resultXml += "<folder>";
//					resultXml += "<folder_header>";
//					resultXml += textToSave;
//					resultXml += "</folder_header>";

//					++openedFolders;

//					break;
//				}

//				case ScenarioTextBlockStyle::FolderFooter: {
//					//
//					// Закрываем папки, если были открыты, то просто корректируем счётчик,
//					// а если открытых нет, то не записываем и конец
//					//
//					if (openedFolders > 0) {
//						--openedFolders;

//						resultXml += "<folder_footer>";
//						resultXml += textToSave;
//						resultXml += "</folder_footer>";
//						resultXml += "</folder>";
//					}
//					break;
//				}

//				default: {
//					break;
//				}
//			}

//			//
//			// Снимем выделение
//			//
//			cursor.clearSelection();
//		}

//	} while (cursor.position() < _endPosition
//			 && !cursor.atEnd());

//	//
//	// Закроем открытые группы
//	//
//	while (openedGroups > 0) {
//		resultXml += "</scene_group>";
//		--openedGroups;
//	}

//	//
//	// Закроем открытые папки
//	//
//	while (openedFolders > 0) {
//		resultXml += "</folder>";
//		--openedFolders;
//	}

//	//
//	// Добавим корневой элемент
//	//
//	resultXml.prepend("<scenario>");
//	resultXml.append("</scenario>");

//	return resultXml.simplified();
//}


//void ScenarioXmlReader::xmlToScenario(const QString& _xml, ScenarioTextEdit* _editor)
//{
//	_editor->setTextUpdateInProgress(true);

//	//
//	// Необходимо ли изменить тип блока, в который вставляется текст
//	//
//	bool needChangeBlockType = false;

//	//
//	// Если под курсором блок с текстом
//	//
//	if (!_editor->textCursor().block().text().simplified().isEmpty()) {
//		//
//		// ... сместим курсор в конец блока, чтобы не разрывать блок вставкой
//		//
//		_editor->moveCursor(QTextCursor::EndOfBlock);
//	} else {
//		needChangeBlockType = true;
//	}

//	QXmlStreamReader reader(_xml);
//	while (!reader.atEnd()) {
//		switch (reader.readNext()) {
//			case QXmlStreamReader::StartElement: {
//				//
//				// Определить тип текущего блока
//				//
//				ScenarioTextBlockStyle::Type tokenType = ScenarioTextBlockStyle::Undefined;
//				QString tokenName = reader.name().toString();
//				if (tokenName == "time_and_place") {
//					tokenType = ScenarioTextBlockStyle::TimeAndPlace;
//				} else if (tokenName == "action") {
//					tokenType = ScenarioTextBlockStyle::Action;
//				} else if (tokenName == "character") {
//					tokenType = ScenarioTextBlockStyle::Character;
//				} else if (tokenName == "parenthetical") {
//					tokenType = ScenarioTextBlockStyle::Parenthetical;
//				} else if (tokenName == "dialog") {
//					tokenType = ScenarioTextBlockStyle::Dialog;
//				} else if (tokenName == "transition") {
//					tokenType = ScenarioTextBlockStyle::Transition;
//				} else if (tokenName == "note") {
//					tokenType = ScenarioTextBlockStyle::Note;
//				} else if (tokenName == "title") {
//					tokenType = ScenarioTextBlockStyle::Title;
//				} else if (tokenName == "simple_text") {
//					tokenType = ScenarioTextBlockStyle::NoprintableText;
//				} else if (tokenName == "scene_group_header") {
//					tokenType = ScenarioTextBlockStyle::SceneGroupHeader;
//				} else if (tokenName == "scene_group_footer") {
//					//
//					// Для блока конца группы нужно сместиться к следующему блоку
//					// этот блок как раз и будет блоком конца группы.
//					// Затем необходимо выделить автоматически сгенерированный текст блока
//					// для того, чтобы заменить его текстом из xml.
//					//
//					_editor->moveCursor(QTextCursor::NextBlock);
//					_editor->moveCursor(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
//				} else if (tokenName == "folder_header") {
//					tokenType = ScenarioTextBlockStyle::FolderHeader;
//				} else if (tokenName == "folder_footer") {
//					//
//					// Для блока конца папки нужно сместиться к следующему блоку
//					// этот блок как раз и будет блоком конца группы.
//					// Затем необходимо выделить автоматически сгенерированный текст блока
//					// для того, чтобы заменить его текстом из xml.
//					//
//					_editor->moveCursor(QTextCursor::NextBlock);
//					_editor->moveCursor(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
//				}

//				//
//				// Если определён тип блока, то обработать его
//				//
//				if (tokenType != ScenarioTextBlockStyle::Undefined) {
//					if (needChangeBlockType) {
//						_editor->changeScenarioBlockType(tokenType);
//						needChangeBlockType = false;
//					} else {
//						_editor->addScenarioBlock(tokenType);
//					}
//				}

//				break;
//			}

//			case QXmlStreamReader::Characters: {
//				_editor->textCursor().insertText(reader.text().toString().simplified());
//				_editor->moveCursor(QTextCursor::EndOfBlock);
//				break;
//			}

//			default: {
//				break;
//			}
//		}
//	}

//	_editor->setTextUpdateInProgress(false);

//	emit _editor->structureChanged();
//}


ScenarioXml::ScenarioXml(ScenarioDocument* _scenario) :
	m_scenario(_scenario)
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
			QString textToSave = cursor.selectedText().simplified();

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

void ScenarioXml::xmlToScenario(int _position, const QString& _xml)
{
	//
	// Необходимо ли изменить тип блока, в который вставляется текст
	//
	bool needChangeBlockType = false;

	//
	// Если под курсором блок с текстом
	//
	QTextCursor cursor(m_scenario->document());
	cursor.setPosition(_position);

	//
	// Если вставка в начало блока
	//
	if (cursor.atBlockStart()) {
		cursor.insertBlock();
		cursor.setPosition(_position);
		needChangeBlockType = true;
	} else if (!cursor.block().text().simplified().isEmpty()) {
		//
		// ... сместим курсор в конец блока, чтобы не разрывать блок вставкой
		//
		cursor.movePosition(QTextCursor::EndOfBlock);
	} else {
		needChangeBlockType = true;
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

					//
					// Если необходимо сменить тип блока, то ни чего не делаем,
					// в противном случае добавляем новый блок в документ
					//
					if (needChangeBlockType) {
						needChangeBlockType = false;
					} else {
						cursor.insertBlock();
					}

					//
					// Установим стиль блока
					//
					cursor.setBlockCharFormat(currentStyle.charFormat());
					cursor.setBlockFormat(currentStyle.blockFormat());
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
}

void ScenarioXml::xmlToScenario(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore, const QString& _xml)
{
	//
	// Определим позицию для вставки данных
	//
	int insertPosition = m_scenario->positionToInsertMime(_insertParent, _insertBefore);

	//
	// Вставка данных
	//
	xmlToScenario(insertPosition, _xml);
}
