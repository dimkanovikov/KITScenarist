#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>

namespace UserInterface {
	class SettingsView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий настройками программы
	 */
	class SettingsManager : public QObject
	{
		Q_OBJECT

	public:
		explicit SettingsManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить состояние окна
		 */
		void loadViewState();

		/**
		 * @brief Сохранить состояние окна
		 */
		void saveViewState();

	signals:
		/**
		 * @brief Обновления настроек
		 */
		/** @{ */
		void applicationSettingsUpdated();
		void scenarioEditSettingsUpdated();
		void navigatorSettingsUpdated();
		void chronometrySettingsUpdated();
		void countersSettingsUpdated();
		/** @} */

	private slots:
		/**
		 * @brief Изменения параметров
		 */
		/** @{ */
		void applicationUseDarkThemeChanged(bool _value);
		void applicationAutosaveChanged(bool _value);
		void applicationAutosaveIntervalChanged(int _value);

		void scenarioEditShowScenesNumbersChanged(bool  _value);
		void scenarioEditPageViewChanged(bool  _value);
		void scenarioEditSpellCheckChanged(bool  _value);
		void scenarioEditSpellCheckLanguageChanged(int _value);
		void scenarioEditTextColorChanged(const QColor& _value);
		void scenarioEditBackgroundColorChanged(const QColor& _value);
		void scenarioEditNonprintableTextColorChanged(const QColor& _value);
		void scenarioEditFolderTextColorChanged(const QColor& _value);
		void scenarioEditFolderBackgroundColorChanged(const QColor& _value);
		void scenarioEditTextColorDarkChanged(const QColor& _value);
		void scenarioEditBackgroundColorDarkChanged(const QColor& _value);
		void scenarioEditNonprintableTextColorDarkChanged(const QColor& _value);
		void scenarioEditFolderTextColorDarkChanged(const QColor& _value);
		void scenarioEditFolderBackgroundColorDarkChanged(const QColor& _value);
		void scenarioEditCurrentStyleChanged(const QString& _value);
		void scenarioEditBlockJumpChanged(const QString& _block, const QString& _tab, const QString& _enter);

		void navigatorShowScenesNumbersChanged(bool  _value);
		void navigatorShowSceneDescriptionChanged(bool _value);
		void navigatorSceneDescriptionIsSceneTextChanged(bool _value);
		void navigatorSceneDescriptionHeightChanged(int _value);

		void chronometryUsedChanged(bool _value);
		void chronometryCurrentTypeChanged();
		void chronometryPagesSecondsChanged(int  _value);
		void chronometryCharactersCharactersChanged(int  _value);
		void chronometryCharactersSecondsChanged(int  _value);
		void chronometryCharactersConsiderSpacesChanged(bool  _value);
		void chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double  _value);
		void chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double  _value);
		void chronometryConfigurableSecondsForParagraphActionChanged(double  _value);
		void chronometryConfigurableSecondsFor50ActionChanged(double  _value);
		void chronometryConfigurableSecondsForParagraphDialogChanged(double  _value);
		void chronometryConfigurableSecondsFor50DialogChanged(double  _value);

		void pagesCounterUsedChanged(bool _value);
		void wordsCounterUsedChanged(bool _value);
		void simbolsCounterUsedChanged(bool _value);
		/** @} */

		/**
		 * @brief Библиотека стилей
		 */
		/** @{ */
		void styleLibraryNewPressed();
		void styleLibraryEditPressed(const QModelIndex& _styleIndex);
		void styleLibraryRemovePressed(const QModelIndex& _styleIndex);
		void styleLibraryLoadPressed();
		void styleLibrarySavePressed(const QModelIndex& _styleIndex);
		/** @} */

	private:
		void storeValue(const QString& _key, bool _value);
		void storeValue(const QString& _key, int _value);
		void storeValue(const QString& _key, double _value);
		void storeValue(const QString& _key, const QString& _value);
		void storeValue(const QString& _key, const QColor& _value);

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
		 * @brief Представление
		 */
		UserInterface::SettingsView* m_view;
	};
}

#endif // SETTINGSMANAGER_H
