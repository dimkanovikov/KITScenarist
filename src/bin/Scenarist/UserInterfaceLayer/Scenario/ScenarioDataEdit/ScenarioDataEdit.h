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

	private slots:
		/**
		 * @brief Изменение параметров сценария
		 */
		/** @{ */
		void aboutNameChanged();
		void aboutAdditionalInfoChanged();
		void aboutGenreChanged();
		void aboutAuthorChanged();
		void aboutContactsChanged();
		void aboutYearChanged();
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
		 * @brief Дополнительная информация
		 */
		QString m_sourceAdditionalInfo;

		/**
		 * @brief Жанр
		 */
		QString m_sourceGenre;

		/**
		 * @brief Автор
		 */
		QString m_sourceAuthor;

		/**
		 * @brief Контакты
		 */
		QString m_sourceContacts;

		/**
		 * @brief Год
		 */
		QString m_sourceYear;

		/**
		 * @brief Текст синопсиса
		 */
		QString m_sourceSourceSynopsis;
	};
}

#endif // SCENARIODATAEDIT_H
