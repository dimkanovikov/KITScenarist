#ifndef SCENARIONAVIGATOR_H
#define SCENARIONAVIGATOR_H

#include <QWidget>

#include <3rd_party/qt_utils/QTreeViewState.h>

class ScenarioTextEdit;
class NavigatorItemsModel;
class QTreeView;


/**
 * @brief Навигатор по проекту сценария
 */
class ScenarioNavigator : public QWidget
{
	Q_OBJECT

public:
	explicit ScenarioNavigator(QWidget* _parent, ScenarioTextEdit* _editor);

private slots:
	/**
	 * @brief Сохранить состояние дерева
	 */
	void aboutStoreTreeViewState();

	/**
	 * @brief Загрузить состояние дерева навигации
	 */
	void aboutRestoreTreeViewState();

private:
	/**
	 * @brief Настройка внешнего вида
	 */
	void initView(ScenarioTextEdit* _editor);

	/**
	 * @brief Настройка соединений для обработки событий
	 */
	void initConnections();

private:
	/**
	 * @brief Дерево проекта
	 */
	NavigatorItemsModel* m_model;

	/**
	 * @brief Виджет для отображения дерева проекта
	 */
	QTreeView* m_view;

	/**
	 * @brief Текущее состояние дерева
	 */
	QTreeViewState m_viewState;
};

#endif // SCENARIONAVIGATOR_H
