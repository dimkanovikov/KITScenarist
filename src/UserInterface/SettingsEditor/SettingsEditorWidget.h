#ifndef SETTINGSEDITORWIDGET_H
#define SETTINGSEDITORWIDGET_H

#include <QWidget>

namespace Ui {
	class SettingsEditorWidget;
}

class QListWidget;
class QStackedWidget;
class QTabWidget;


/**
 * @brief Класс настройки параметров приложения
 */
class SettingsEditorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SettingsEditorWidget(QWidget *parent = 0);
	~SettingsEditorWidget();

signals:
	/**
	 * @brief Включена/отключена проверка орфографии
	 */
	void useSpellCheckingChanged(bool);

private slots:
	/**
	 * @brief Показать настройки выбранной категории
	 */
	void aboutShowCategorySettings(int _categoryRow);

	/**
	 * @brief Сохранить настройки
	 */
	void saveSettings();

private:
	void initView();
	void initConnections();
	void initSaveSettingsConnections();

	/**
	 * @brief Загрузить настройки
	 */
	void loadSettings();

	QListWidget* categories();
	QStackedWidget* categoriesWidgets();

	Ui::SettingsEditorWidget* m_ui;
};

#endif // SETTINGSEDITORWIDGET_H
