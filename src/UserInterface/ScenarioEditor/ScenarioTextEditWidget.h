#ifndef SCENARIOTEXTEDITWIDGET_H
#define SCENARIOTEXTEDITWIDGET_H

#include <QFrame>

class QComboBox;
class QRadioButton;
class QLabel;
class ScenarioTextEdit;


/**
 * @brief Виджет редактора сценария
 */
class ScenarioTextEditWidget : public QFrame
{
	Q_OBJECT

public:
	explicit ScenarioTextEditWidget(QWidget *parent = 0);

	ScenarioTextEdit* editor();

signals:


private slots:
	/**
	 * @brief Обновить интерфейс в соответствии с текущим стилем блока в редакторе
	 */
	void aboutUpdateStyle();

	/**
	 * @brief Установить в редакторе выбранный стиль
	 */
	void aboutSetStyle();

	/**
	 * @brief Обновить хронометраж
	 */
	void aboutUpdateChronometry();

private:
	void initView();
	void initConnections();
	void initStyleSheet();

	QComboBox* types();
	QRadioButton* textMode();
	QLabel* chron();
	ScenarioTextEdit* scenarioEdit();

	/**
	 * @brief Создать виджет для заполнения верхней панели
	 */
	QWidget* spacerWidget(int _width, bool _leftBordered, bool _rightBordered) const;

private:
	QComboBox* m_types;
	QRadioButton* m_textMode;
	QLabel* m_chron;
	ScenarioTextEdit* m_scenarioEdit;
};

#endif // SCENARIOTEXTEDITWIDGET_H
