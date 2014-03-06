#ifndef SCENARIONAVIGATOR_H
#define SCENARIONAVIGATOR_H

#include <QWidget>

class QTreeView;
class QLabel;
class QAbstractItemModel;

namespace UserInterface
{
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
		 * @brief Количество сцен в сценарии
		 */
		QLabel* m_scenesCount;

		/**
		 * @brief Дерево навигации
		 */
		QTreeView* m_navigationTree;
	};
}

#endif // SCENARIONAVIGATOR_H
