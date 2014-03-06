#include "ScenarioTextDocument.h"

#include "ScenarioXml.h"

using namespace BusinessLogic;


ScenarioTextDocument::ScenarioTextDocument(QObject *parent, ScenarioXml* _xmlHandler) :
	QTextDocument(parent),
	m_xmlHandler(_xmlHandler)
{
}

QString ScenarioTextDocument::mimeFromSelection(int _startPosition, int _endPosition) const
{
	QString mime;

	if (m_xmlHandler != 0) {
		//
		// Скорректируем позиции в случае необходимости
		//
		if (_startPosition > _endPosition) {
			qSwap(_startPosition, _endPosition);
		}

		mime = m_xmlHandler->scenarioToXml(_startPosition, _endPosition);
	}

	return mime;
}

void ScenarioTextDocument::insertFromMime(int _insertPosition, const QString& _mimeData)
{
	if (m_xmlHandler != 0) {
		m_xmlHandler->xmlToScenario(_insertPosition, _mimeData);
	}
}
