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

	private slots:
		/**
		 * @brief Изменился стиль экспорта
		 */
		void aboutExportStyleChanged(const QString& _styleName);

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
		UserInterface::ExportDialog* m_exportDialog;
	};
}

#endif // EXPORTMANAGER_H
