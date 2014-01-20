#ifndef SCENARIOEDITORWIDGET_H
#define SCENARIOEDITORWIDGET_H

#include <QWidget>

class QComboBox;
class QRadioButton;
class QLabel;
class ScenarioTextEdit;


/**
 * @brief Виджет редактора сценария
 */
class ScenarioEditorWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ScenarioEditorWidget(QWidget *parent = 0);

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

#endif // SCENARIOEDITORWIDGET_H
