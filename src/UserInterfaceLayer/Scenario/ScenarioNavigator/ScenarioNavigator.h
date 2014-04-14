#ifndef SCENARIONAVIGATOR_H
#define SCENARIONAVIGATOR_H

#include <QWidget>

class QTreeView;
class QLabel;
class QAbstractItemModel;

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

	protected:
		/**
		 * @brief Переопределяется чтобы отлавливать нажатия Ctrl+Z и Ctrl+Shift+Z в дереве
		 */
		bool eventFilter(QObject* _watched, QEvent* _event);

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
		 * @brief Заголовок навигатора
		 */
		QLabel* m_title;

		/**
		 * @brief Префикс счётчика сцен
		 */
		QLabel* m_scenesCountTitle;

		/**
		 * @brief Количество сцен в сценарии
		 */
		QLabel* m_scenesCount;

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
