#ifndef SCENARIONAVIGATORMANAGER_H
#define SCENARIONAVIGATORMANAGER_H

#include <QObject>
#include <QModelIndex>

class QLabel;

namespace BusinessLogic {
	class ScenarioModel;
	class ScenarioModelFiltered;
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
		explicit ScenarioNavigatorManager(QObject* _parent, QWidget* _parentWidget, bool _isDraft = false);

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

		/**
		 * @brief Снять выделение
		 */
		void clearSelection();

		/**
		 * @brief Установить видимость черновика
		 */
		void setDraftVisible(bool _visible);

		/**
		 * @brief Установить видимость заметок
		 */
		void setNoteVisible(bool _visible);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
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
		void undoRequest();

		/**
		 * @brief Запрос повтора действия
		 */
		void redoRequest();

	private slots:
		/**
		 * @brief Добавить элемент после выбранного
		 */
		void aboutAddItem(const QModelIndex& _index);

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
		 * @brief Прокси для модели сценирия
		 */
		BusinessLogic::ScenarioModelFiltered* m_scenarioModelProxy;

		/**
		 * @brief Дерево навигации
		 */
		UserInterface::ScenarioNavigator* m_navigator;

		/**
		 * @brief Диалог добавления элемента
		 */
		UserInterface::ScenarioItemDialog* m_addItemDialog;

		/**
		 * @brief Является ли навигатором по черновику
		 */
		bool m_isDraft;
	};
}

#endif // SCENARIONAVIGATORMANAGER_H
