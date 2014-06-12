#ifndef SCENARIONAVIGATORMANAGER_H
#define SCENARIONAVIGATORMANAGER_H

#include <QObject>
#include <QModelIndex>

class QLabel;

namespace BusinessLogic {
	class ScenarioModel;
}

namespace UserInterface {
	class ScenarioNavigator;
	class ScenarioItemDialog;
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
		 * @brief Запрос на добавление элемента
		 */
		void addItem(const QModelIndex& _afterItemIndex, const QString& _itemHeader, int _itemType);

		/**
		 * @brief Запрос на удаление элемента
		 */
		void removeItems(const QModelIndexList& _itemIndex);

		/**
		 * @brief Активирована сцена
		 */
		void sceneChoosed(const QModelIndex& _index);
		void sceneChoosed(int atPosition);

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
		 * @brief Добавить элемент после выбранного
		 */
		void aboutAddItem(const QModelIndex& _index);

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

		/**
		 * @brief Настроить соединения модели
		 */
		/** @{ */
		void connectModel();
		void disconnectModel();
		/** @} */

	private:

		/**
		 * @brief Модель сценария
		 */
		BusinessLogic::ScenarioModel* m_scenarioModel;

		/**
		 * @brief Дерево навигации
		 */
		UserInterface::ScenarioNavigator* m_navigator;

		/**
		 * @brief Диалог добавления элемента
		 */
		UserInterface::ScenarioItemDialog* m_addItemDialog;
	};
}

#endif // SCENARIONAVIGATORMANAGER_H
