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
		QString scenarioName() const;

		/**
		 * @brief Установить название
		 */
		void setScenarioName(const QString& _name);

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
		 * @brief Запрос на построение синопсиса из сцен
		 */
		void buildSynopsisFromScenes();

	private slots:
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
	};
}

#endif // SCENARIODATAEDIT_H
