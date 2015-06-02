#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QWidget>

class QAbstractItemModel;
class HierarchicalHeaderView;
class QSplitter;
class TabBar;

namespace Ui {
	class SettingsView;
}


namespace UserInterface
{
	class SettingsView : public QWidget
	{
		Q_OBJECT

	public:
		explicit SettingsView(QWidget *parent = 0);
		~SettingsView();

		/**
		 * @brief Разделитель представления
		 */
		QSplitter* splitter() const;

		/**
		 * @brief Установить модель алгоритма смены блоков
		 */
		void setBlocksSettingsModel(QAbstractItemModel* _model, QAbstractItemModel* _modelForDelegate);

		/**
		 * @brief Установить модель стилей
		 */
		void setTemplatesModel(QAbstractItemModel* _model);

		/**
		 * @brief Текущий тип рассчёта хронометража
		 */
		int chronometryCurrentType() const;

		/**
		 * @brief Установка параметров
		 */
		/** @{ */
		void setApplicationUseDarkTheme(bool _value);
		void setApplicationAutosave(bool);
		void setApplicationAutosaveInterval(int);
		void setApplicationSaveBackups(bool _save);
		void setApplicationSaveBackupsFolder(const QString& _folder);

		void setScenarioEditShowScenesNumbers(bool _value);
		void setScenarioEditPageView(bool _value);
		void setScenarioEditSpellCheck(bool _value);
		void setScenarioEditSpellCheckLanguage(int _value);
		void setScenarioEditTextColor(const QColor& _color);
		void setScenarioEditBackgroundColor(const QColor& _color);
		void setScenarioEditNonprintableTexColor(const QColor& _color);
		void setScenarioEditFolderTextColor(const QColor& _color);
		void setScenarioEditFolderBackgroundColor(const QColor& _color);
		void setScenarioEditTextColorDark(const QColor& _color);
		void setScenarioEditBackgroundColorDark(const QColor& _color);
		void setScenarioEditNonprintableTexColorDark(const QColor& _color);
		void setScenarioEditFolderTextColorDark(const QColor& _color);
		void setScenarioEditFolderBackgroundColorDark(const QColor& _color);
		void setScenarioEditCurrentTemplate(const QString& _styleName);

		void setNavigatorShowScenesNumbers(bool _value);
		void setNavigatorShowSceneDescription(bool _value);
		void setNavigatorSceneDescriptionIsSceneText(bool _value);
		void setNavigatorSceneDescriptionHeight(int _value);

		void setChronometryUsed(bool _value);
		void setChronometryCurrentType(int _value);
		void setChronometryPagesSeconds(int  _value);
		void setChronometryCharactersCharacters(int  _value);
		void setChronometryCharactersSeconds(int  _value);
		void setChronometryCharactersConsiderSpaces(bool  _value);
		void setChronometryConfigurableSecondsForParagraphTimeAndPlace(double  _value);
		void setChronometryConfigurableSecondsFor50TimeAndPlace(double  _value);
		void setChronometryConfigurableSecondsForParagraphAction(double  _value);
		void setChronometryConfigurableSecondsFor50Action(double  _value);
		void setChronometryConfigurableSecondsForParagraphDialog(double  _value);
		void setChronometryConfigurableSecondsFor50Dialog(double  _value);

		void setPagesCounterUsed(bool _value);
		void setWordsCounterUsed(bool _value);
		void setSimbolsCounterUsed(bool _value);
		/** @} */

	signals:
		/**
		 * @brief Сбросить пользовательские настройки
		 */
		void resetSettings();

		/**
		 * @brief Изменения параметров
		 */
		/** @{ */
		void applicationUseDarkThemeChanged(bool);
		void applicationAutosaveChanged(bool);
		void applicationAutosaveIntervalChanged(int);
		void applicationSaveBackupsChanged(bool);
		void applicationSaveBackupsFolderChanged(const QString&);

		void scenarioEditShowScenesNumbersChanged(bool);
		void scenarioEditPageViewChanged(bool);
		void scenarioEditSpellCheckChanged(bool);
		void scenarioEditSpellCheckLanguageChanged(int);
		void scenarioEditTextColorChanged(const QColor&);
		void scenarioEditBackgroundColorChanged(const QColor&);
		void scenarioEditNonprintableTextColorChanged(const QColor&);
		void scenarioEditFolderTextColorChanged(const QColor&);
		void scenarioEditFolderBackgroundColorChanged(const QColor&);
		void scenarioEditTextColorDarkChanged(const QColor&);
		void scenarioEditBackgroundColorDarkChanged(const QColor&);
		void scenarioEditNonprintableTextColorDarkChanged(const QColor&);
		void scenarioEditFolderTextColorDarkChanged(const QColor&);
		void scenarioEditFolderBackgroundColorDarkChanged(const QColor&);
		void scenarioEditCurrentTemplateChanged(const QString&);
		void scenarioEditBlockSettingsChanged(const QString& _block, const QString& _shortcut,
			const QString& _tab, const QString& _enter, const QString& _changeTab,
			const QString& _changeEnter);

		void navigatorShowScenesNumbersChanged(bool);
		void navigatorShowSceneDescriptionChanged(bool);
		void navigatorSceneDescriptionIsSceneTextChanged(bool);
		void navigatorSceneDescriptionHeightChanged(int);

		void chronometryUsedChanged(bool);
		void chronometryCurrentTypeChanged();
		void chronometryPagesSecondsChanged(int);
		void chronometryCharactersCharactersChanged(int);
		void chronometryCharactersSecondsChanged(int);
		void chronometryCharactersConsiderSpaces(bool);
		void chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double);
		void chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double);
		void chronometryConfigurableSecondsForParagraphActionChanged(double);
		void chronometryConfigurableSecondsFor50ActionChanged(double);
		void chronometryConfigurableSecondsForParagraphDialogChanged(double);
		void chronometryConfigurableSecondsFor50DialogChanged(double);

		void pagesCounterUsedChanged(bool);
		void wordsCounterUsedChanged(bool);
		void simbolsCounterUsedChanged(bool);
		/** @} */

		/**
		 * @brief Библиотека стилей
		 */
		/** @{ */
		void templateLibraryNewPressed();
		void templateLibraryEditPressed(const QModelIndex& _templateIndex);
		void templateLibraryRemovePressed(const QModelIndex& _templateIndex);
		void templateLibraryLoadPressed();
		void templateLibrarySavePressed(const QModelIndex& _templateIndex);
		/** @} */

	private slots:
		/**
		 * @brief Обработка смены языка для проверки орфографии
		 */
		void aboutScenarioEditSpellCheckLanguageChanged();

		/**
		 * @brief Сменилась выбранная цветовая схема
		 */
		void aboutColorThemeChanged();

		/**
		 * @brief Запрос на открытие диалога выбора папки сохранения резервных копий
		 */
		void aboutBrowseSaveBackupsFolder();

		/**
		 * @brief Сменился блок в настройках переходов
		 */
		void aboutBlockJumpChanged(const QModelIndex& _topLeft, const QModelIndex& _bottomRight);

		/**
		 * @brief Выбрать цвет для параметров редактора сценария
		 */
		/** @{ */
		void aboutScenarioEditChooseTextColor();
		void aboutScenarioEditChooseBackgroundColor();
		void aboutScenarioEditChooseNonprintableTextColor();
		void aboutScenarioEditChooseFolderTextColor();
		void aboutScenarioEditChooseFolderBackgroundColor();
		void aboutScenarioEditChooseTextColorDark();
		void aboutScenarioEditChooseBackgroundColorDark();
		void aboutScenarioEditChooseNonprintableTextColorDark();
		void aboutScenarioEditChooseFolderTextColorDark();
		void aboutScenarioEditChooseFolderBackgroundColorDark();
		/** @} */

		/**
		 * @brief Библиотека шаблонов
		 */
		/** @{ */
		void aboutEditTemplatePressed();
		void aboutRemoveTemplatePressed();
		void aboutSaveTemplatePressed();
		void aboutApplyTemplatePressed();
		/** @} */

	private:
		/**
		 * @brief Сменить цвет для элемента вызвав диалог выбора цвета
		 */
		void setColorFor(QWidget* _colorPicker);

		/**
		 * @brief Установить цвет для элемента
		 */
		void setColorFor(QWidget* _colorPicker, const QColor& _newColor);

	private:
		/**
		 * @brief Настроить данные формы
		 */
		void initData();

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
		 * @brief Форма
		 */
		Ui::SettingsView *ui;

		/**
		 * @brief Переключатель вкладок для настроек редактора сценария
		 */
		TabBar* m_scenarioEditorTabs;

		/**
		 * @brief Шапка таблицы переходов между блоками
		 */
		HierarchicalHeaderView* m_jumpsTableHeader;
	};
}

#endif // SETTINGSVIEW_H
