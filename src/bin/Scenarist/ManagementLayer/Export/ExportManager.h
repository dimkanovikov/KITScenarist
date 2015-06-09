#ifndef EXPORTMANAGER_H
#define EXPORTMANAGER_H

#include <QObject>

namespace BusinessLogic {
	class ScenarioDocument;
}

namespace UserInterface {
	class ExportDialog;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий экспортом
	 */
	class ExportManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ExportManager(QObject* _parent, QWidget* _parentWidget);

		/**
		 * @brief Экспортировать документ
		 */
		void exportScenario(BusinessLogic::ScenarioDocument* _scenario);

		/**
		 * @brief Предварительный просмотр документа
		 */
		void printPreviewScenario(BusinessLogic::ScenarioDocument* _scenario);

		/**
		 * @brief Загрузим настройки экспорта для текущего проекта
		 */
		void loadCurrentProjectSettings(const QString& _projectPath);

		/**
		 * @brief Сохраним настройки экспорта для текущего проекта
		 */
		void saveCurrentProjectSettings(const QString& _projectPath);

	signals:
		/**
		 * @brief Было изменено название сценария
		 */
		void scenarioNameChanged(const QString& _name);

		/**
		 * @brief Изменены какие-либо данные титульного листа сценария
		 */
		void scenarioTitleListDataChanged();

	private slots:
		/**
		 * @brief Изменился стиль экспорта
		 */
		void aboutExportStyleChanged(const QString& _styleName);

		/**
		 * @brief Запрос предварительного просмотра из самого диалога
		 */
		void aboutPrintPreview();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Загрузить в диалог настройки экспорта
		 */
		void initExportDialog();

		/**
		 * @brief Сохранить информацию о титульном листе, если изменилась
		 */
		void saveTitleListInfo();

	private:
		/**
		 * @brief Текущий экспортируемый сценарий
		 */
		BusinessLogic::ScenarioDocument* m_currentScenario;

		/**
		 * @brief Диалог экспорта
		 */
		UserInterface::ExportDialog* m_exportDialog;
	};
}

#endif // EXPORTMANAGER_H
