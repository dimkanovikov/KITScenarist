#ifndef STATISTICSMANAGER_H
#define STATISTICSMANAGER_H

#include <QObject>

class QTextDocument;

namespace UserInterface {
	class StatisticsView;
}

namespace BusinessLogic {
	class StatisticsParameters;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий статистикой
	 */
	class StatisticsManager : public QObject
	{
		Q_OBJECT

	public:
		explicit StatisticsManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

		/**
		 * @brief Ихменился текст сценария, а значит нужно обновить флаг перезагрузки сценария
		 */
		void scenarioTextChanged();

	public slots:
		/**
		 * @brief Установить экспортированный сценарий, по которому будет считаться статистика
		 */
		void setExportedScenario(QTextDocument* _scenario);

	signals:
		/**
		 * @brief Запрос на формирование нового экспортированного сценария
		 */
		void needNewExportedScenario();

	private slots:
		/**
		 * @brief Сформировать отчёт
		 */
		void aboutMakeReport(const BusinessLogic::StatisticsParameters& _parameters);

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
		 * @brief Представление для страницы со статистикой
		 */
		UserInterface::StatisticsView* m_view;

		/**
		 * @brief Текст сценария
		 */
		QTextDocument* m_exportedScenario;

		/**
		 * @brief Флаг обозначающий необходимость обновить текст сценария перед построением отчёта
		 */
		bool m_needUpdateScenario;
	};
}

#endif // STATISTICSMANAGER_H
