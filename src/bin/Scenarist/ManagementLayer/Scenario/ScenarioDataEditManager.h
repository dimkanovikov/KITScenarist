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
		 * @brief Дополнительная информация
		 */
		/** @{ */
		QString scenarioAdditionalInfo() const;
		void setScenarioAdditionalInfo(const QString& _additionalInfo);
		/** @} */

		/**
		 * @brief Жанр
		 */
		/** @{ */
		QString scenarioGenre() const;
		void setScenarioGenre(const QString& _genre);
		/** @} */

		/**
		 * @brief Автор
		 */
		/** @{ */
		QString scenarioAuthor() const;
		void setScenarioAuthor(const QString _author);
		/** @} */

		/**
		 * @brief Контактная информация
		 */
		/** @{ */
		QString scenarioContacts() const;
		void setScenarioContacts(const QString& _contacts);
		/** @} */

		/**
		 * @brief Год
		 */
		/** @{ */
		QString scenarioYear() const;
		void setScenarioYear(const QString& _year);
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
		void scenarioAdditionalInfoChanged();
		void scenarioGenreChanged();
		void scenarioAuthorChanged();
		void scenarioContactsChanged();
		void scenarioYearChanged();
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
