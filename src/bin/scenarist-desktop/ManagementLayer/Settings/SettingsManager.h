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
		 * @brief Включить выключить двухпанельный режим
		 */
		void setUseTwoPanelMode(bool _use);

	signals:
		/**
		 * @brief Обновления настроек
		 */
		/** @{ */
		void applicationSettingsUpdated();
		void cardsSettingsUpdated();
		void scenarioEditSettingsUpdated();
		void navigatorSettingsUpdated();
		void chronometrySettingsUpdated();
		void countersSettingsUpdated();
		/** @} */

	private slots:
		/**
		 * @brief Сбросить настройки
		 */
		void aboutResetSettings();

		/**
		 * @brief Изменения параметров
		 */
		/** @{ */
		void applicationLanguageChanged(int _value);
		void applicationUseDarkThemeChanged(bool _value);
		void applicationAutosaveChanged(bool _value);
		void applicationAutosaveIntervalChanged(int _value);
		void applicationSaveBackupsChanged(bool _value);
        void applicationSaveBackupsFolderChanged(const QString& _value);
        void applicationCompactModeChanged(bool _enable);
		void applicationTwoPanelModeChanged(bool _value);
		void applicationModuleResearchChanged(bool _value);
		void applicationModuleCardsChanged(bool _value);
        void applicationModuleScenarioChanged(bool _value);
		void applicationModuleStatisticsChanged(bool _value);

		void cardsUseCorkboardBackgroundChanged(bool);
		void cardsBackgroundColorChanged(const QColor&);
		void cardsBackgroundColorDarkChanged(const QColor&);

		void scenarioEditPageViewChanged(bool  _value);
		void scenarioEditShowScenesNumbersChanged(bool  _value);
		void scenarioEditHighlightCurrentLineChanged(bool _value);
		void scenarioEditCapitalizeFirstWordChanged(bool _value);
		void scenarioEditCorrectDoubleCapitalsChanged(bool _value);
		void scenarioEditReplaceThreeDotsChanged(bool _value);
		void scenarioEditSmartQuotesChanged(bool _value);
		void scenarioEditSpellCheckChanged(bool _value);
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
		void scenarioEditCurrentTemplateChanged(const QString& _value);
		void scenarioEditAutoJumpToNextBlockChanged(bool _value);
		void scenarioEditShowSuggestionsInEmptyBlocksChanged(bool _value);
		void scenarioEditBlockSettingsChanged(const QString& _block, const QString& _shortcut,
			const QString& _tab, const QString& _enter, const QString& _changeTab,
			const QString& _changeEnter);
		void scenarioEditReviewUseWordHighlightChanged(bool _value);

		void navigatorShowScenesNumbersChanged(bool  _value);
		void navigatorShowSceneTitleChanged(bool _value);
		void navigatorShowSceneDescriptionChanged(bool _value);
		void navigatorSceneDescriptionIsSceneTextChanged(bool _value);
		void navigatorSceneDescriptionHeightChanged(int _value);

		void chronometryUsedChanged(bool _value);
		void chronometryCurrentTypeChanged();
		void chronometryPagesSecondsChanged(int  _value);
		void chronometryCharactersCharactersChanged(int  _value);
		void chronometryCharactersSecondsChanged(int  _value);
		void chronometryCharactersConsiderSpacesChanged(bool  _value);
		void chronometryConfigurableSecondsForParagraphSceneHeadingChanged(double  _value);
		void chronometryConfigurableSecondsFor50SceneHeadingChanged(double  _value);
		void chronometryConfigurableSecondsForParagraphActionChanged(double  _value);
		void chronometryConfigurableSecondsFor50ActionChanged(double  _value);
		void chronometryConfigurableSecondsForParagraphDialogChanged(double  _value);
		void chronometryConfigurableSecondsFor50DialogChanged(double  _value);

		void pagesCounterUsedChanged(bool _value);
		void wordsCounterUsedChanged(bool _value);
		void simbolsCounterUsedChanged(bool _value);
		/** @} */

		/**
		 * @brief Библиотека шаблонов
		 */
		/** @{ */
		void templateLibraryNewPressed();
		void templateLibraryEditPressed(const QModelIndex& _templateIndex);
		void templateLibraryRemovePressed(const QModelIndex& _templateIndex);
		void templateLibraryLoadPressed();
		void templateLibrarySavePressed(const QModelIndex& _templateIndex);
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
