#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QWidget>

class QAbstractItemModel;
class QSplitter;

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
		 * @brief Установить модель стилей
		 */
		void setStylesModel(QAbstractItemModel* _model);

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

		void setScenarioEditPageView(bool _value);
		void setScenarioEditSpellCheck(bool _value);
		void setScenarioEditSpellCheckLanguage(int _value);
		void setScenarioEditTextColor(const QColor& _color);
		void setScenarioEditBackgroundColor(const QColor& _color);
		void setScenarioEditNonprintableTexColor(const QColor& _color);
		void setScenarioEditFolderTextColor(const QColor& _color);
		void setScenarioEditFolderBackgroundColor(const QColor& _color);
		void setScenarioEditCurrentStyle(const QString& _styleName);

		void setNavigatorShowScenesNumbers(bool  _value);

		void setChronometryCurrentType(int _value);
		void setChronometryPagesSeconds(int  _value);
		void setChronometryCharactersCharacters(int  _value);
		void setChronometryCharactersSeconds(int  _value);
		void setChronometryConfigurableSecondsForParagraphTimeAndPlace(double  _value);
		void setChronometryConfigurableSecondsFor50TimeAndPlace(double  _value);
		void setChronometryConfigurableSecondsForParagraphAction(double  _value);
		void setChronometryConfigurableSecondsFor50Action(double  _value);
		void setChronometryConfigurableSecondsForParagraphDialog(double  _value);
		void setChronometryConfigurableSecondsFor50Dialog(double  _value);
		/** @} */

	signals:
		/**
		 * @brief Изменения параметров
		 */
		/** @{ */
		void applicationUseDarkThemeChanged(bool);
		void applicationAutosaveChanged(bool);
		void applicationAutosaveIntervalChanged(int);

		void scenarioEditPageViewChanged(bool);
		void scenarioEditSpellCheckChanged(bool);
		void scenarioEditSpellCheckLanguageChanged(int);
		void scenarioEditTextColorChanged(const QColor&);
		void scenarioEditBackgroundColorChanged(const QColor&);
		void scenarioEditNonprintableTextColorChanged(const QColor&);
		void scenarioEditFolderTextColorChanged(const QColor&);
		void scenarioEditFolderBackgroundColorChanged(const QColor&);
		void scenarioEditCurrentStyleChanged(const QString&);

		void navigatorShowScenesNumbersChanged(bool);
		void navigatorShowScenesTextChanged(bool);

		void chronometryCurrentTypeChanged();
		void chronometryPagesSecondsChanged(int);
		void chronometryCharactersCharactersChanged(int);
		void chronometryCharactersSecondsChanged(int);
		void chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double);
		void chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double);
		void chronometryConfigurableSecondsForParagraphActionChanged(double);
		void chronometryConfigurableSecondsFor50ActionChanged(double);
		void chronometryConfigurableSecondsForParagraphDialogChanged(double);
		void chronometryConfigurableSecondsFor50DialogChanged(double);
		/** @} */

		/**
		 * @brief Библиотека стилей
		 */
		/** @{ */
		void newStylePressed();
		void editStylePressed(const QModelIndex& _styleIndex);
		void removeStylePressed(const QModelIndex& _styleIndex);
		void loadStylePressed();
		void saveStylePressed(const QModelIndex& _styleIndex);
		/** @} */

	private slots:
		/**
		 * @brief Обработка смены языка для проверки орфографии
		 */
		void aboutScenarioEditSpellCheckLanguageChanged();

		/**
		 * @brief Выбрать цвет для параметров редактора сценария
		 */
		/** @{ */
		void aboutScenarioEditChooseTextColor();
		void aboutScenarioEditChooseBackgroundColor();
		void aboutScenarioEditChooseNonprintableTextColor();
		void aboutScenarioEditChooseFolderTextColor();
		void aboutScenarioEditChooseFolderBackgroundColor();
		/** @} */

		/**
		 * @brief Библиотека стилей
		 */
		/** @{ */
		void aboutEditStylePressed();
		void aboutRemoveStylePressed();
		void aboutSaveStylePressed();
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
	};
}

#endif // SETTINGSVIEW_H
