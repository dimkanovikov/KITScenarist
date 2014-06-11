#ifndef SCENARIONAVIGATOR_H
#define SCENARIONAVIGATOR_H

#include <QWidget>
#include <QModelIndex>

class QAbstractItemModel;
class QLabel;
class QToolButton;
class QTreeView;

namespace UserInterface
{
	class ScenarioNavigatorItemDelegate;


	/**
	 * @brief Навигатор по сценарию
	 */
	class ScenarioNavigator : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioNavigator(QWidget *parent = 0);

		/**
		 * @brief Установить количество сцен
		 */
		void setScenesCount(int _scenesCount);

		/**
		 * @brief Установить модель навигации
		 */
		void setModel(QAbstractItemModel* _model);

		/**
		 * @brief Установить текущий элемент
		 */
		void setCurrentIndex(const QModelIndex& _index);

		/**
		 * @brief Настроить отображение номеров сцен
		 */
		void setShowSceneNumber(bool _show);

	signals:
		/**
		 * @brief Запрос на добавление элемента
		 */
		void addItem(const QModelIndex& _itemIndex);

		/**
		 * @brief Запрос на удаление элемента
		 */
		void removeItems(const QModelIndexList& _itemIndex);

		/**
		 * @brief Активирована сцена
		 */
		void sceneChoosed(const QModelIndex& _sceneIndex);

		/**
		 * @brief Запрос отмены действия
		 */
		void undoPressed();

		/**
		 * @brief Запрос повтора действия
		 */
		void redoPressed();

	protected:
		/**
		 * @brief Переопределяется чтобы отлавливать нажатия Ctrl+Z и Ctrl+Shift+Z в дереве
		 */
		bool eventFilter(QObject* _watched, QEvent* _event);

	private slots:
		/**
		 * @brief Обработка запроса на добаление элемента
		 */
		void aboutAddItem();

		/**
		 * @brief Обработка запроса на удаление элемента
		 */
		void aboutRemoveItem();

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
		/**
		 * @brief Префикс счётчика сцен
		 */
		QLabel* m_scenesCountTitle;

		/**
		 * @brief Количество сцен в сценарии
		 */
		QLabel* m_scenesCount;

		/**
		 * @brief Кнопка удаления локации
		 */
		QToolButton* m_addItem;

		/**
		 * @brief Кнопка обновления списка локаций
		 */
		QToolButton* m_removeItem;

		/**
		 * @brief Окончание панели инструментов
		 */
		QLabel* m_endTitle;

		/**
		 * @brief Дерево навигации
		 */
		QTreeView* m_navigationTree;

		/**
		 * @brief Делегат дерева
		 */
		ScenarioNavigatorItemDelegate* m_navigationTreeDelegate;
	};
}

#endif // SCENARIONAVIGATOR_H
