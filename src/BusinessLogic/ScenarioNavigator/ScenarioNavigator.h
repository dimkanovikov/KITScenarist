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

	/**
	 * @brief Получить кол-во сцен
	 */
	int scenesCount() const;

signals:
	/**
	 * @brief Была обновлена структура сценария
	 */
	void structureChanged();

private slots:
	/**
	 * @brief Выделить элемент в дереве в соответствии с редактируемой сценой
	 */
	void aboutSelectItemForCurrentScene();

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
	void initConnections(ScenarioTextEdit* _editor);

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
