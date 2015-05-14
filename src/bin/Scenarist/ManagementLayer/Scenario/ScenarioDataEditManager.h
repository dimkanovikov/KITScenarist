#ifndef SCENARIODATAEDITMANAGER_H
#define SCENARIODATAEDITMANAGER_H

#include <QObject>

namespace UserInterface {
	class ScenarioDataEdit;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий редактором информации о сценарие
	 */
	class ScenarioDataEditManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioDataEditManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* toolbar() const;
		QWidget* view() const;

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

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

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
		 * @brief Редактор информации о сценарие
		 */
		UserInterface::ScenarioDataEdit* m_view;

	};
}

#endif // SCENARIODATAEDITMANAGER_H
