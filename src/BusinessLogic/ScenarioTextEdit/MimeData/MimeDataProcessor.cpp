#include "MimeDataProcessor.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioXml.h>

#include <QTextCursor>
#include <QTextBlock>
#include <QMimeData>
#include <QXmlStreamReader>


QString MimeDataProcessor::SCENARIO_MIME_TYPE = "application/x-scenarius";

QString MimeDataProcessor::createMimeFromSelection(const ScenarioTextEdit* _editor)
{
	//
	// Сохраним позиции курсора в выделении
	//
	QTextCursor cursor = _editor->textCursor();
	int startCursorPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
	int endCursorPosition = qMax(cursor.selectionStart(), cursor.selectionEnd());

	return ScenarioXmlWriter::scenarioToXml(_editor, startCursorPosition, endCursorPosition);
}

void MimeDataProcessor::insertFromMime(ScenarioTextEdit* _editor, const QMimeData* _mimeData)
{
	QString mimeDataText = _mimeData->data(SCENARIO_MIME_TYPE);

	ScenarioXmlReader::xmlToScenario(mimeDataText, _editor);
}
