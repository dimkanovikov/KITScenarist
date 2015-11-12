#ifndef SCENARIONAVIGATORMANAGER_H
#define SCENARIONAVIGATORMANAGER_H

#include <QObject>
#include <QModelIndex>

namespace BusinessLogic {
	class ScenarioModel;
	class ScenarioModelFiltered;
}

namespace UserInterface {
	class ScenarioNavigatorView;
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
		explicit ScenarioNavigatorManager(QObject* _parent, QWidget* _parentWidget, bool _isDraft = false);

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Представление
		 */
		QWidget* view() const;

		/**
		 * @brief Задать название сценария
		 */
		void setScenarioName(const QString& _name);

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

		/**
		 * @brief Снять выделение
		 */
		void clearSelection();

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Запрос на отображение текста сценария
		 */
		void showTextRequested();

		/**
		 * @brief Запрос на добавление элемента
		 */
		void addItem(const QModelIndex& _afterItemIndex, int _itemType, const QString& _header,
			const QColor& _color, const QString& _description);

		/**
		 * @brief Запрос на удаление элемента
		 */
		void removeItems(const QModelIndexList& _indexes);

		/**
		 * @brief Запрос на установку цвета элемента
		 */
		void setItemColors(const QModelIndex& _indexes, const QString& _colors);

		/**
		 * @brief Показать/скрыть заметки к сцене
		 */
		void showHideDraft();

		/**
		 * @brief Показать/скрыть заметки к сцене
		 */
		void showHideNote();

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
//		/**
//		 * @brief Добавить элемент после выбранного
//		 */
//		void aboutAddItem(const QModelIndex& _index);

		/**
		 * @brief Удалить выбранные элементы
		 */
		void aboutRemoveItems(const QModelIndexList& _indexes);

		/**
		 * @brief Установить цвета элемента
		 */
		void aboutSetItemColors(const QModelIndex& _index, const QString& _colors);

		/**
		 * @brief Выбрана сцена
		 */
		void aboutSceneChoosed(const QModelIndex& _index);

		/**
		 * @brief Обновить информацию о модели
		 */
		void aboutModelUpdated();

	private:
        /**
         * @brief Настроить данные
         */
        void initData();

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

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Представление навигатора
		 */
		UserInterface::ScenarioNavigatorView* m_view;

		/**
		 * @brief Модель сценария
		 */
		BusinessLogic::ScenarioModel* m_scenarioModel;

		/**
		 * @brief Прокси для модели сценирия
		 */
		BusinessLogic::ScenarioModelFiltered* m_scenarioModelProxy;

//		/**
//		 * @brief Диалог добавления элемента
//		 */
//		UserInterface::ScenarioItemDialog* m_addItemDialog;

		/**
		 * @brief Является ли навигатором по черновику
		 */
		bool m_isDraft;
	};
}

#endif // SCENARIONAVIGATORMANAGER_H
