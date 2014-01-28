#include "MimeDataProcessor.h"

#include "../ScenarioTextEdit.h"
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>
#include "../ScenarioXml.h"

#include <QTextCursor>
#include <QTextBlock>
#include <QMimeData>
#include <QXmlStreamReader>

using namespace BusinessLogic;


QString MimeDataProcessor::SCENARIO_MIME_TYPE = "application/x-scenarius";

QString MimeDataProcessor::createMimeFromSelection(const ScenarioTextEdit* _editor)
{
	//
	// Сохраним позиции курсора в выделении
	//
	QTextCursor cursor = _editor->textCursor();
	int startCursorPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
	int endCursorPosition = qMax(cursor.selectionStart(), cursor.selectionEnd());

	return ScenarioXmlWriter::scenarioToXml(
				_editor,
				startCursorPosition,
				endCursorPosition);
}

QString MimeDataProcessor::createMimeFromBlocks(const ScenarioTextEdit* _editor, const QTextBlock& _start, const QTextBlock& _end)
{
	return ScenarioXmlWriter::scenarioToXml(
				_editor,
				_start.position(),
				_end.position() + _end.length() - 1);
}

void MimeDataProcessor::insertFromMime(ScenarioTextEdit* _editor, const QMimeData* _mimeData)
{
	QString mimeDataText = _mimeData->data(SCENARIO_MIME_TYPE);

	ScenarioXmlReader::xmlToScenario(mimeDataText, _editor);
}
