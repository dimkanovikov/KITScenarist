#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>

class QToolButton;
class SideBar;
class QStackedLayout;
class StartUpWidget;
class ScenarioEditorWidget;
class CharactersEditorWidget;
class LocationsEditorWidget;
class SettingsEditorWidget;


/**
 * @brief Виджет приложения
 */
class ApplicationWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ApplicationWidget(QWidget *parent = 0);

signals:

private slots:
	/**
	 * @brief Создать новый сценарий
	 */
	void aboutCreateNewScenario();

	/**
	 * @brief Открыть существующий сценарий
	 */
	void aboutOpenScenario();

	/**
	 * @brief Сохранить сценарий
	 */
	void aboutSaveScenario();

private:
	void initView();
	void initConnections();
	void initStyleSheet();

	/**
	 * @brief Кнопка меню
	 */
	QToolButton* menu();

	/**
	 * @brief Боковая панель
	 */
	SideBar* tabs();

	/**
	 * @brief Стэк с виджетами редакторами
	 */
	QStackedLayout* tabsWidgets();

	/**
	 * @brief Виджет стартового экрана
	 */
	StartUpWidget* startUp();

	/**
	 * @brief Редактор сценария
	 */
	ScenarioEditorWidget* scenarioEditor();

	/**
	 * @brief Редактор персонажей
	 */
	CharactersEditorWidget* charactersEditor();

	/**
	 * @brief Редактор локаций
	 */
	LocationsEditorWidget* locationsEditor();

	/**
	 * @brief Редактор настроек
	 */
	SettingsEditorWidget* settingsEditor();

private:
	SideBar* m_tabs;

	QStackedLayout* m_tabsWidgets;

	StartUpWidget* m_startUp;
	ScenarioEditorWidget* m_scenarioEditor;
	CharactersEditorWidget* m_charactersEditor;
	LocationsEditorWidget* m_locationsEditor;
	SettingsEditorWidget* m_settingsEditor;

	/**
	 * @brief Пункты меню
	 */
	QToolButton* m_menu;
	QAction* m_menuCreateNew;
	QAction* m_menuOpen;
	QAction* m_menuSave;
	QAction* m_menuSaveAs;
	QAction* m_menuSaveAsPdf;
	QAction* m_menuSaveAsRtf;
	QAction* m_menuExit;
};

#endif // APPLICATIONWIDGET_H
