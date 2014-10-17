#ifndef SCENARIODATAEDIT_H
#define SCENARIODATAEDIT_H

#include <QWidget>

namespace Ui {
	class ScenarioDataEdit;
}


namespace UserInterface
{
	/**
	 * @brief Редактор информации о сценарие
	 */
	class ScenarioDataEdit : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioDataEdit(QWidget* _parent = 0);
		~ScenarioDataEdit();

		QWidget* toolbar() const;

		/**
		 * @brief Очистить от старых данных
		 */
		void clear();

		/**
		 * @brief Название сценария
		 */
		/** @{ */
		QString scenarioName() const;
		void setScenarioName(const QString& _name);
		/** @} */

		/**
		 * @brief Синопсис сценария
		 */
		QString scenarioSynopsis() const;

		/**
		 * @brief Установить синопсис
		 */
		void setScenarioSynopsis(const QString& _synopsis);

		/**
		 * @brief Установить синопсис собранный из сцен
		 */
		void setScenarioSynopsisFromScenes(const QString& _synopsis);

	signals:
		/**
		 * @brief Сигралы об изменении параметров
		 */
		/** @{ */
		void scenarioNameChanged();
		void scenarioSynopsisChanged();
		/** @} */

		/**
		 * @brief Запрос на построение синопсиса из сцен
		 */
		void buildSynopsisFromScenes();

	private slots:
		/**
		 * @brief Изменение параметров сценария
		 */
		/** @{ */
		void aboutNameChanged();
		void aboutSourceSynopsisChanged();
		/** @} */

		/**
		 * @brief Сменился текущий выбранный синопсис (с исходного на посценный или наоборот)
		 */
		void aboutCurrentSynopsisChanged();

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
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		Ui::ScenarioDataEdit* ui;

		/**
		 * @brief Исходные значения используется для контроля изменений
		 */

		/**
		 * @brief Название сценария
		 */
		QString m_sourceName;

		/**
		 * @brief Текст синопсиса
		 */
		QString m_sourceSourceSynopsis;
	};
}

#endif // SCENARIODATAEDIT_H
