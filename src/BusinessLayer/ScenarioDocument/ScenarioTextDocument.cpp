#include "ScenarioTextDocument.h"

#include "ScenarioXml.h"

using namespace BusinessLogic;


ScenarioTextDocument::ScenarioTextDocument(QObject *parent, ScenarioXml* _xmlHandler) :
	QTextDocument(parent),
	m_xmlHandler(_xmlHandler)
{
	Q_ASSERT(m_xmlHandler);
}

QString ScenarioTextDocument::mimeFromSelection(int _startPosition, int _endPosition) const
{
	//
	// Скорректируем позиции в случае необходимости
	//
	if (_startPosition > _endPosition) {
		qSwap(_startPosition, _endPosition);
	}

	return m_xmlHandler->scenarioToXml(_startPosition, _endPosition);
}

void ScenarioTextDocument::insertFromMime(int _insertPosition, const QString& _mimeData)
{
	m_xmlHandler->xmlToScenario(_insertPosition, _mimeData);
}
