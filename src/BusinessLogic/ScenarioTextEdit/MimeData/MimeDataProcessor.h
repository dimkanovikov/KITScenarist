#ifndef MIMEDATAPROCESSOR_H
#define MIMEDATAPROCESSOR_H

#include <QString>

class ScenarioTextEdit;
class QMimeData;


/**
 * @brief Класс для записи/чтения данных из майм-типа сценария
 */
class MimeDataProcessor
{
public:
	static QString SCENARIO_MIME_TYPE;

	static QString createMimeFromSelection(const ScenarioTextEdit* _editor);
	static void insertFromMime(ScenarioTextEdit* _editor, const QMimeData* _mimeData);
};

#endif // MIMEDATAPROCESSOR_H
