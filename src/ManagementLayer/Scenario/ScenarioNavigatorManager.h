#ifndef SCENARIONAVIGATORMANAGER_H
#define SCENARIONAVIGATORMANAGER_H

#include <QObject>

class QLabel;

namespace BusinessLogic {
	class ScenarioModel;
}

namespace UserInterface {
	class ScenarioNavigator;
}

namespace ManagementLayer
{
	/**
	 * @brief Управляющий навигацией по сценарию
	 */
	class ScenarioNavigatorManager : public QObject
	{
		Q_OBJECT
	public:
		explicit ScenarioNavigatorManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Установить модель документа сценария
		 */
		void setNavigationModel(BusinessLogic::ScenarioModel* _model);

		/**
		 * @brief Перезагрузить настройки навигатора
		 */
		void reloadNavigatorSettings();

		/**
		 * @brief Установить текущий элемент
		 */
		void setCurrentIndex(const QModelIndex& _index);

	signals:
		/**
		 * @brief Активирована сцена
		 */
		void sceneChoosed(const QModelIndex& _index);

		/**
		 * @brief Запрос отмены действия
		 */
		void undoPressed();

		/**
		 * @brief Запрос повтора действия
		 */
		void redoPressed();

	private slots:
		/**
		 * @brief Обновить информацию о модели
		 */
		void aboutModelUpdated();

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
		 * @brief Модель сценария
		 */
		BusinessLogic::ScenarioModel* m_scenarioModel;

		/**
		 * @brief Дерево навигации
		 */
		UserInterface::ScenarioNavigator* m_navigator;
	};
}

#endif // SCENARIONAVIGATORMANAGER_H
