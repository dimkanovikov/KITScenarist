#ifndef IMPORTMANAGER_H
#define IMPORTMANAGER_H

#include <QObject>

namespace BusinessLogic {
	class ScenarioDocument;
}

namespace UserInterface {
	class ImportDialog;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий импортом
	 */
class ImportManager : public QObject
{
	Q_OBJECT

public:
	explicit ImportManager(QObject* _parent, QWidget* _parentWidget);

	/**
	 * @brief Импортировать сценарий
	 */
	void importScenario(BusinessLogic::ScenarioDocument* _scenario, int _cursorPosition);

private:
	/**
	 * @brief Настроить представление
	 */
	void initView();

	/**
	 * @brief Настроить соединения
	 */
	void initConnections();

private:
	/**
	 * @brief Диалог экспорта
	 */
	UserInterface::ImportDialog* m_importDialog;
};
}

#endif // IMPORTMANAGER_H
