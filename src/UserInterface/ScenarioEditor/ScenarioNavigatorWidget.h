#ifndef SCENARIONAVIGATORWIDGET_H
#define SCENARIONAVIGATORWIDGET_H

#include <QFrame>

class ScenarioTextEdit;
class QLabel;
class ScenarioNavigator;


/**
 * @brief Виджет навигатора по сценарию
 */
class ScenarioNavigatorWidget : public QFrame
{
	Q_OBJECT

public:
	explicit ScenarioNavigatorWidget(QWidget *parent, ScenarioTextEdit* _editor);

signals:

private slots:
	/**
	 * @brief Обновить кол-во сцен
	 */
	void aboutUpdateScenesCount();

private:
	void initView();
	void initConnections();
	void initStyleSheet();

	ScenarioTextEdit* editor();
	QLabel* title();
	QLabel* scenesCounter();
	ScenarioNavigator* navigator();

	/**
	 * @brief Создать виджет для заполнения верхней панели
	 */
	QWidget* spacerWidget(int _width, bool _leftBordered, bool _rightBordered) const;

private:
	ScenarioTextEdit* m_editor;
	QLabel* m_title;
	QLabel* m_scenesCounter;
	ScenarioNavigator* m_navigator;

};

#endif // SCENARIONAVIGATORWIDGET_H
