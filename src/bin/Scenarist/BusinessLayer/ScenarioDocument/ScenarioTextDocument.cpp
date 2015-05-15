#include "ScenarioTextDocument.h"

#include "ScenarioXml.h"

#include <Domain/ScenarioChange.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioChangeStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/DiffMatchPatchHelper.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <QCryptographicHash>

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

	/**
	 * @brief Подготовить майм для вставки из заданного xml
	 */
	static QString makeMimeFromXml(const QString& _xml) {
		const QString XML_HEADER = "<?xml version=\"1.0\"?>";
		const QString SCENARIO_HEADER = "<scenario>";
		const QString SCENARIO_FOOTER = "</scenario>";

		QString mimeXml = _xml;
		if (!mimeXml.contains(XML_HEADER)) {
			if (!mimeXml.contains(SCENARIO_HEADER)) {
				mimeXml.prepend(SCENARIO_HEADER);
			}
			mimeXml.prepend(XML_HEADER);
		}
		if (!mimeXml.endsWith(SCENARIO_FOOTER)) {
			mimeXml.append(SCENARIO_FOOTER);
		}
		return mimeXml;
	}

	/**
	 * @brief Сохранить изменение
	 */
	static Domain::ScenarioChange* saveChange(const QString& _undoPatch, const QString& _redoPatch) {
		const QString username =
				PasswordStorage::load(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"application/user-name",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					);
		return DataStorageLayer::StorageFacade::scenarioChangeStorage()->append(username, _undoPatch, _redoPatch);
	}
}


ScenarioTextDocument::ScenarioTextDocument(QObject *parent, ScenarioXml* _xmlHandler) :
	QTextDocument(parent),
	m_xmlHandler(_xmlHandler),
	m_isPatchApplyProcessed(false)
{
}

void ScenarioTextDocument::load(const QString& _scenarioXml)
{
	//
	// Если xml не задан сформируем его пустой аналог
	//
	QString scenarioXml = _scenarioXml;
	if (scenarioXml.isEmpty()) {
		scenarioXml = m_xmlHandler->defaultXml();
	}

	//
	// Загружаем проект
	//
	m_xmlHandler->xmlToScenario(0, scenarioXml);
	m_lastScenarioXml = scenarioXml;
	m_lastScenarioXmlHash = ::textMd5Hash(scenarioXml);

	m_undoStack.clear();
	m_redoStack.clear();
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

void ScenarioTextDocument::applyPatch(const QString& _patch)
{
	emit beforePatchApply();
	m_isPatchApplyProcessed = true;


	//
	// Определим xml для применения патча
	//
	const QString currentXml = m_xmlHandler->scenarioToXml();
	QPair<DiffMatchPatchHelper::ChangeXml, DiffMatchPatchHelper::ChangeXml> xmlsForUpdate;
	xmlsForUpdate = DiffMatchPatchHelper::changedXml(currentXml, _patch);

	//
	// Выделяем текст сценария, соответствующий xml для обновления
	//
	QTextCursor cursor(this);
	setCursorPosition(cursor, xmlsForUpdate.first.plainPos);
	const int selectionEndPos = xmlsForUpdate.first.plainPos + xmlsForUpdate.first.plainLength;
	setCursorPosition(cursor, selectionEndPos, QTextCursor::KeepAnchor);

	//
	// Замещаем его обновлённым
	//
	cursor.beginEditBlock();
	cursor.removeSelectedText();
	m_xmlHandler->xmlToScenario(xmlsForUpdate.first.plainPos,
		::makeMimeFromXml(xmlsForUpdate.second.xml));
	cursor.endEditBlock();

	//
	// Запомним новый текст
	//
	m_lastScenarioXml = m_xmlHandler->scenarioToXml();
	m_lastScenarioXmlHash = ::textMd5Hash(m_lastScenarioXml);


	m_isPatchApplyProcessed = false;
	emit afterPatchApply();
}

Domain::ScenarioChange* ScenarioTextDocument::saveChanges()
{
	Domain::ScenarioChange* change = 0;

	if (!m_isPatchApplyProcessed) {
		const QString newScenarioXml = m_xmlHandler->scenarioToXml();
		const QByteArray newScenarioXmlHash = ::textMd5Hash(newScenarioXml);

		//
		// Если текущий текст сценария отличается от последнего сохранённого
		//
		if (newScenarioXmlHash != m_lastScenarioXmlHash) {
			//
			// Сформируем изменения
			//
			const QString undoPatch = DiffMatchPatchHelper::makePatchXml(newScenarioXml, m_lastScenarioXml);
			const QString redoPatch = DiffMatchPatchHelper::makePatchXml(m_lastScenarioXml, newScenarioXml);

			//
			// Сохраним изменения
			//
			change = ::saveChange(undoPatch, redoPatch);

			//
			// Запомним новый текст
			//
			m_lastScenarioXml = newScenarioXml;
			m_lastScenarioXmlHash = newScenarioXmlHash;

			//
			// Корректируем стеки последних действий
			//
			m_undoStack.append(change);
			m_redoStack.clear();
		}
	}

	return change;
}

void ScenarioTextDocument::undoReimpl()
{
	saveChanges();

	if (!m_undoStack.isEmpty()) {
		Domain::ScenarioChange* change = m_undoStack.takeLast();
		m_redoStack.append(change);
		applyPatch(change->undoPatch());

		//
		// Сохраним изменения
		//
		Domain::ScenarioChange* newChange = ::saveChange(change->redoPatch(), change->undoPatch());
		newChange->setIsDraft(change->isDraft());
	}
}

void ScenarioTextDocument::redoReimpl()
{
	if (!m_redoStack.isEmpty()) {
		Domain::ScenarioChange* change = m_redoStack.takeLast();
		m_undoStack.append(change);
		applyPatch(change->redoPatch());

		//
		// Сохраним изменения
		//
		Domain::ScenarioChange* newChange = ::saveChange(change->undoPatch(), change->redoPatch());
		newChange->setIsDraft(change->isDraft());
	}
}

bool ScenarioTextDocument::isUndoAvailableReimpl() const
{
	return !m_undoStack.isEmpty();
}

bool ScenarioTextDocument::isRedoAvailableReimpl() const
{
	return !m_redoStack.isEmpty();
}

void ScenarioTextDocument::setCursorPosition(QTextCursor& _cursor, int _position, QTextCursor::MoveMode _moveMode)
{
	//
	// Нормальное позиционирование
	//
	if (_position > 0 && _position < characterCount()) {
		_cursor.setPosition(_position, _moveMode);
	}
	//
	// Для отрицательного ни чего не делаем, оставляем курсор в нуле
	//
	else if (_position < 0) {
		_cursor.movePosition(QTextCursor::Start, _moveMode);
	}
	//
	// Для очень большого, просто помещаем в конец документа
	//
	else {
		_cursor.movePosition(QTextCursor::End, _moveMode);
	}
}
