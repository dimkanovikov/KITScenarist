#ifndef RESEARCHMANAGER_H
#define RESEARCHMANAGER_H

#include <QObject>
#include <QMap>

namespace UserInterface {
	class ResearchView;
	class ResearchItemDialog;
}

namespace Domain {
	class Research;
}

namespace BusinessLogic {
	class ResearchModel;
	class ResearchModelItem;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий разработкой
	 */
	class ResearchManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ResearchManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

		/**
		 * @brief Очистить все загруженные данные
		 */
		void closeCurrentProject();

		/**
		 * @brief Обновить параметры
		 */
		void updateSettings();

		/**
		 * @brief Сохранить разработки проекта
		 */
		void saveResearch();

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

		/**
		 * @brief Получить название сценария
		 */
		QString scenarioName() const;

		/**
		 * @brief Получить данные о сценарии
		 */
		QMap<QString, QString> scenarioData() const;

	signals:
		/**
		 * @brief Было изменено название проекта
		 */
		void scenarioNameChanged(const QString& _name);

		/**
		 * @brief Была изменена разарботка
		 */
		void researchChanged();

	private:
		/**
		 * @brief Добавить разработку
		 */
		void addResearch(const QModelIndex& _index);

		/**
		 * @brief Изменить разработку
		 */
		void editResearch(const QModelIndex& _index);

		/**
		 * @brief Удалить разработку
		 */
		void removeResearch(const QModelIndex& _index);

		/**
		 * @brief Показать контекстное меню навигатора
		 */
		void showNavigatorContextMenu(const QModelIndex& _index, const QPoint& _pos);

		/**
		 * @brief Обновить данные сценария
		 */
		void updateScenarioData(const QString& _key, const QString& _value);

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
		 * @brief Представление
		 */
		UserInterface::ResearchView* m_view;

		/**
		 * @brief Диалог добавления элемента разработки
		 */
		UserInterface::ResearchItemDialog* m_dialog;

		/**
		 * @brief Данные сценария
		 */
		QMap<QString, QString> m_scenarioData;

		/**
		 * @brief Модель данных о разработке
		 */
		BusinessLogic::ResearchModel* m_model;

		/**
		 * @brief Текущий элемент разработки
		 */
		BusinessLogic::ResearchModelItem* m_currentResearchItem;
		Domain::Research* m_currentResearch;
	};
}

#endif // RESEARCHMANAGER_H
