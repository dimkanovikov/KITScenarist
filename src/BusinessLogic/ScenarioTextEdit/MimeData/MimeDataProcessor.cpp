#include "MimeDataProcessor.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>

#include <QTextCursor>
#include <QTextBlock>
#include <QMimeData>
#include <QXmlStreamReader>


QString MimeDataProcessor::SCENARIO_MIME_TYPE = "application/x-scenarius";

QString MimeDataProcessor::createMimeFromSelection(const ScenarioTextEdit* _editor)
{
	QString resultXml;

	//
	// Сохраним позиции курсора в выделении
	//
	QTextCursor cursor = _editor->textCursor();
	int startCursorPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
	int endCursorPosition = qMax(cursor.selectionStart(), cursor.selectionEnd());

	//
	// Переместимся к началу выделения
	//
	cursor.setPosition(startCursorPosition);

	//
	// Подсчитаем кол-во незакрытых групп и закроем, если необходимо
	//
	int openedGroups = 0;

	do {
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

		ScenarioTextBlockStyle::Type currentType = _editor->scenarioBlockType(cursor.block());

		//
		// Курсор в конце текущего блока
		// или в конце выделения
		//
		if (cursor.atBlockEnd()
			|| cursor.position() == endCursorPosition) {
			//
			// Получить текст под курсором
			//
			QString textToSave = cursor.selectedText();

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

				case ScenarioTextBlockStyle::SceneGroupHeader: {
					resultXml += "<scene_group>";
					resultXml += "<header>";
					resultXml += textToSave;
					resultXml += "</header>";

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

						resultXml += "<footer>";
						resultXml += textToSave;
						resultXml += "</footer>";
						resultXml += "</scene_group>";
					}
					break;
				}

				case ScenarioTextBlockStyle::SimpleText: {
					resultXml += "<simple_text>";
					resultXml += textToSave;
					resultXml += "</simple_text>";
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

	} while (cursor.position() <= endCursorPosition
			 && !cursor.atEnd());

	//
	// Закроем открытые группы
	//
	while (openedGroups > 0) {
		resultXml += "</scene_group>";
		--openedGroups;
	}


	//
	// Добавим заголовок
	//
	resultXml.prepend("<scenario>");
	resultXml.append("</scenario>");

	return resultXml.simplified();
}

void MimeDataProcessor::insertFromMime(ScenarioTextEdit* _editor, const QMimeData* _mimeData)
{
	QString mimeDataText = _mimeData->data(SCENARIO_MIME_TYPE);

	QXmlStreamReader reader(mimeDataText);
	while (!reader.atEnd()) {
		switch (reader.readNext()) {
			case QXmlStreamReader::StartElement: {
				QString tokenName = reader.name().toString();
				if (tokenName == "time_and_place") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::TimeAndPlace);
				} else if (tokenName == "action") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::Action);
				} else if (tokenName == "character") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::Character);
				} else if (tokenName == "parenthetical") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::Parenthetical);
				} else if (tokenName == "dialog") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::Dialog);
				} else if (tokenName == "transition") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::Transition);
				} else if (tokenName == "note") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::Note);
				} else if (tokenName == "title") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::Title);
				} else if (tokenName == "header") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::SceneGroupHeader);
				} else if (tokenName == "footer") {
					_editor->moveCursor(QTextCursor::NextBlock);
					_editor->moveCursor(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
//					_editor->addScenarioBlock(ScenarioTextBlockStyle::SceneGroupFooter);
				} else if (tokenName == "simple_text") {
					_editor->addScenarioBlock(ScenarioTextBlockStyle::SimpleText);
				}
				break;
			}

			case QXmlStreamReader::Characters: {
				_editor->textCursor().insertText(reader.text().toString().simplified());
				_editor->moveCursor(QTextCursor::EndOfBlock);
				break;
			}

			default: {
				break;
			}
		}
	}
}
