#include "SettingsManager.h"
#include "SettingsStylesManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/Chronometry/PagesChronometer.h>
#include <BusinessLayer/Chronometry/CharactersChronometer.h>
#include <BusinessLayer/Chronometry/ConfigurableChronometer.h>

#include <UserInterfaceLayer/Settings/SettingsView.h>

#include <QFileDialog>
#include <QSplitter>
#include <QStandardItemModel>

using ManagementLayer::SettingsManager;
using ManagementLayer::SettingsStylesManager;
using BusinessLogic::ScenarioStyleFacade;
using UserInterface::SettingsView;

namespace {
	/**
	 * @brief Расширение файла стиля сценария
	 */
	const QString SCENARIO_STYLE_FILE_EXTENSION = "kitss";
}


SettingsManager::SettingsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new SettingsView(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* SettingsManager::view() const
{
	return m_view;
}

void SettingsManager::loadViewState()
{
	m_view->splitter()->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"application/settings/geometry",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_view->splitter()->restoreState(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"application/settings/state",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
}

void SettingsManager::saveViewState()
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/settings/geometry", m_view->splitter()->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/settings/state", m_view->splitter()->saveState().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

void SettingsManager::applicationUseDarkThemeChanged(bool _value)
{
	storeValue("application/use-dark-theme", _value);
}

void SettingsManager::applicationAutosaveChanged(bool _value)
{
	storeValue("application/autosave", _value);
}

void SettingsManager::applicationAutosaveIntervalChanged(int _value)
{
	storeValue("application/autosave-interval", _value);
}

void SettingsManager::scenarioEditPageViewChanged(bool _value)
{
	storeValue("scenario-editor/page-view", _value);
}

void SettingsManager::scenarioEditSpellCheckChanged(bool _value)
{
	storeValue("scenario-editor/spell-checking", _value);
}

void SettingsManager::scenarioEditSpellCheckLanguageChanged(int _value)
{
	storeValue("scenario-editor/spell-checking-language", _value);
}

void SettingsManager::scenarioEditTextColorChanged(const QColor&_value)
{
	storeValue("scenario-editor/text-color", _value);
}

void SettingsManager::scenarioEditBackgroundColorChanged(const QColor& _value)
{
	storeValue("scenario-editor/background-color", _value);
}

void SettingsManager::scenarioEditNonprintableTextColorChanged(const QColor& _value)
{
	storeValue("scenario-editor/nonprintable-text-color", _value);
}

void SettingsManager::scenarioEditFolderTextColorChanged(const QColor& _value)
{
	storeValue("scenario-editor/folder-text-color", _value);
}

void SettingsManager::scenarioEditFolderBackgroundColorChanged(const QColor& _value)
{
	storeValue("scenario-editor/folder-background-color", _value);
}

void SettingsManager::scenarioEditCurrentStyleChanged(const QString& _value)
{
	storeValue("scenario-editor/current-style", _value);
}

void SettingsManager::navigatorShowScenesNumbersChanged(bool _value)
{
	storeValue("navigator/show-scenes-numbers", _value);
}

void SettingsManager::chronometryCurrentTypeChanged()
{
	QString chronometryType;
	switch (m_view->chronometryCurrentType()) {
		case 0: {
			chronometryType = BusinessLogic::PagesChronometer().name();
			break;
		}

		case 1: {
			chronometryType = BusinessLogic::CharactersChronometer().name();
			break;
		}

		case 2: {
			chronometryType = BusinessLogic::ConfigurableChronometer().name();
			break;
		}
	}
	storeValue("chronometry/current-chronometer-type", chronometryType);
}

void SettingsManager::chronometryPagesSecondsChanged(int _value)
{
	storeValue("chronometry/pages/seconds", _value);
}

void SettingsManager::chronometryCharactersCharactersChanged(int _value)
{
	storeValue("chronometry/characters/characters", _value);
}

void SettingsManager::chronometryCharactersSecondsChanged(int _value)
{
	storeValue("chronometry/characters/seconds", _value);
}

void SettingsManager::chronometryCharactersConsiderSpacesChanged(bool _value)
{
	storeValue("chronometry/characters/consider-spaces", _value);
}

void SettingsManager::chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-paragraph/time-and-place", _value);
}

void SettingsManager::chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-every-50/time-and-place", _value);
}

void SettingsManager::chronometryConfigurableSecondsForParagraphActionChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-paragraph/action", _value);
}

void SettingsManager::chronometryConfigurableSecondsFor50ActionChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-every-50/action", _value);
}

void SettingsManager::chronometryConfigurableSecondsForParagraphDialogChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-paragraph/dialog", _value);
}

void SettingsManager::chronometryConfigurableSecondsFor50DialogChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-every-50/dialog", _value);
}

void SettingsManager::styleLibraryNewPressed()
{
	SettingsStylesManager stylesManager(this, m_view);
	stylesManager.newStyle();
}

void SettingsManager::styleLibraryEditPressed(const QModelIndex& _styleIndex)
{
	//
	// Определим название стиля к изменению
	//
	const int STYLE_NAME_COLUMN = 0;
	QModelIndex styleNameIndex = _styleIndex.sibling(_styleIndex.row(), STYLE_NAME_COLUMN);
	QString styleToEditName = ScenarioStyleFacade::stylesList()->data(styleNameIndex).toString();

	//
	// Изменим стиль
	//
	SettingsStylesManager stylesManager(this, m_view);
	stylesManager.editStyle(styleToEditName);
}

void SettingsManager::styleLibraryRemovePressed(const QModelIndex& _styleIndex)
{
	//
	// Определим название стиля к удалению
	//
	const int STYLE_NAME_COLUMN = 0;
	QModelIndex styleNameIndex = _styleIndex.sibling(_styleIndex.row(), STYLE_NAME_COLUMN);
	QString styleToDeleteName = ScenarioStyleFacade::stylesList()->data(styleNameIndex).toString();

	//
	// Удалим стиль
	//
	ScenarioStyleFacade::removeStyle(styleToDeleteName);
}

void SettingsManager::styleLibraryLoadPressed()
{
	//
	// Выбрать файл для загрузки
	//
	QString styleFilePath =
			QFileDialog::getOpenFileName(m_view, tr("Choose file to load"), QDir::homePath(),
				tr("Scenario Style Files (*.%1)").arg(SCENARIO_STYLE_FILE_EXTENSION));

	if (!styleFilePath.isEmpty()) {
		//
		// Сохранить стиль в библиотеке
		//
		ScenarioStyleFacade::saveStyle(styleFilePath);
	}
}

void SettingsManager::styleLibrarySavePressed(const QModelIndex& _styleIndex)
{
	//
	// Определим стиль
	//
	const int STYLE_NAME_COLUMN = 0;
	QModelIndex styleNameIndex = _styleIndex.sibling(_styleIndex.row(), STYLE_NAME_COLUMN);
	QString styleToSaveName = ScenarioStyleFacade::stylesList()->data(styleNameIndex).toString();

	//
	// Выбрать файл для сохранения
	//
	QString styleFilePath =
			QFileDialog::getSaveFileName(m_view, tr("Choose file to save"), QDir::homePath(),
				tr("Scenario Style Files (*.%1)").arg(SCENARIO_STYLE_FILE_EXTENSION));

	//
	// Сохраним стиль в файл
	//
	if (!styleFilePath.isEmpty()) {
		if (!styleFilePath.endsWith(SCENARIO_STYLE_FILE_EXTENSION)) {
			styleFilePath += "." + SCENARIO_STYLE_FILE_EXTENSION;
		}
		ScenarioStyleFacade::style(styleToSaveName).saveToFile(styleFilePath);
	}
}

void SettingsManager::storeValue(const QString& _key, bool _value)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				_key, _value ? "1" : "0", DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void SettingsManager::storeValue(const QString& _key, int _value)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				_key, QString::number(_value), DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void SettingsManager::storeValue(const QString& _key, double _value)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				_key, QString::number(_value), DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void SettingsManager::storeValue(const QString& _key, const QString& _value)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				_key, _value, DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void SettingsManager::storeValue(const QString& _key, const QColor& _value)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				_key, _value.name(), DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void SettingsManager::initView()
{
	//
	// Загрузить библиотеку стилей
	//
	m_view->setStylesModel(BusinessLogic::ScenarioStyleFacade::stylesList());

	//
	// Загрузить настройки
	//

	//
	// Настройки приложения
	//
	m_view->setApplicationUseDarkTheme(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/use-dark-theme",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setApplicationAutosave(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "application/autosave",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setApplicationAutosaveInterval(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "application/autosave-interval",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);

	//
	// Настройки текстового редактора
	//
	m_view->setScenarioEditPageView(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/page-view",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setScenarioEditSpellCheck(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/spell-checking",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setScenarioEditSpellCheckLanguage(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/spell-checking-language",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	// ... цвета
	m_view->setScenarioEditTextColor(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/text-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditBackgroundColor(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/background-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditNonprintableTexColor(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/nonprintable-text-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditFolderTextColor(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/folder-text-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditFolderBackgroundColor(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/folder-background-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	// ... текущий стиль
	m_view->setScenarioEditCurrentStyle(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/current-style",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);


	//
	// Настройки навигатора
	//
	m_view->setNavigatorShowScenesNumbers(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scenes-numbers",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);

	//
	// Настройки хронометража
	//

	// ... текущая система
	QString chronometryType =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"chronometry/current-chronometer-type",
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	int chronometryTypeValue = 0;
	if (chronometryType == BusinessLogic::PagesChronometer().name()) {
		chronometryTypeValue = 0;
	} else if (chronometryType == BusinessLogic::CharactersChronometer().name()) {
		chronometryTypeValue = 1;
	} else {
		chronometryTypeValue = 2;
	}
	m_view->setChronometryCurrentType(chronometryTypeValue);

	// ... параметры систем
	m_view->setChronometryPagesSeconds(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/pages/seconds",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setChronometryCharactersCharacters(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/characters/characters",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setChronometryCharactersSeconds(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/characters/seconds",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setChronometryCharactersConsiderSpaces(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/characters/consider-spaces",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setChronometryConfigurableSecondsForParagraphTimeAndPlace(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-paragraph/time-and-place",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_view->setChronometryConfigurableSecondsFor50TimeAndPlace(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-every-50/time-and-place",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_view->setChronometryConfigurableSecondsForParagraphAction(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-paragraph/action",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_view->setChronometryConfigurableSecondsFor50Action(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-every-50/action",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_view->setChronometryConfigurableSecondsForParagraphDialog(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-paragraph/dialog",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_view->setChronometryConfigurableSecondsFor50Dialog(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-every-50/dialog",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
}

void SettingsManager::initConnections()
{
	//
	// Сохранение изменений параметров
	//
	connect(m_view, SIGNAL(applicationUseDarkThemeChanged(bool)), this, SLOT(applicationUseDarkThemeChanged(bool)));
	connect(m_view, SIGNAL(applicationAutosaveChanged(bool)), this, SLOT(applicationAutosaveChanged(bool)));
	connect(m_view, SIGNAL(applicationAutosaveIntervalChanged(int)), this, SLOT(applicationAutosaveIntervalChanged(int)));

	connect(m_view, SIGNAL(scenarioEditPageViewChanged(bool)), this, SLOT(scenarioEditPageViewChanged(bool)));
	connect(m_view, SIGNAL(scenarioEditSpellCheckChanged(bool)), this, SLOT(scenarioEditSpellCheckChanged(bool)));
	connect(m_view, SIGNAL(scenarioEditSpellCheckLanguageChanged(int)), this, SLOT(scenarioEditSpellCheckLanguageChanged(int)));
	connect(m_view, SIGNAL(scenarioEditTextColorChanged(QColor)), this, SLOT(scenarioEditTextColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditBackgroundColorChanged(QColor)), this, SLOT(scenarioEditBackgroundColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditNonprintableTextColorChanged(QColor)), this, SLOT(scenarioEditNonprintableTextColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditFolderTextColorChanged(QColor)), this, SLOT(scenarioEditFolderTextColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditFolderBackgroundColorChanged(QColor)), this, SLOT(scenarioEditFolderBackgroundColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditCurrentStyleChanged(QString)), this, SLOT(scenarioEditCurrentStyleChanged(QString)));

	connect(m_view, SIGNAL(navigatorShowScenesNumbersChanged(bool)), this, SLOT(navigatorShowScenesNumbersChanged(bool)));

	connect(m_view, SIGNAL(chronometryCurrentTypeChanged()), this, SLOT(chronometryCurrentTypeChanged()));
	connect(m_view, SIGNAL(chronometryPagesSecondsChanged(int)), this, SLOT(chronometryPagesSecondsChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersCharactersChanged(int)), this, SLOT(chronometryCharactersCharactersChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersSecondsChanged(int)), this, SLOT(chronometryCharactersSecondsChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersConsiderSpaces(bool)), this, SLOT(chronometryCharactersConsiderSpacesChanged(bool)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double)),
			this, SLOT(chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double)),
			this, SLOT(chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphActionChanged(double)),
			this, SLOT(chronometryConfigurableSecondsForParagraphActionChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50ActionChanged(double)),
			this, SLOT(chronometryConfigurableSecondsFor50ActionChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphDialogChanged(double)),
			this, SLOT(chronometryConfigurableSecondsForParagraphDialogChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50DialogChanged(double)),
			this, SLOT(chronometryConfigurableSecondsFor50DialogChanged(double)));

	//
	// Уведомления об обновлении секции параметров
	//
	connect(m_view, SIGNAL(applicationUseDarkThemeChanged(bool)), this, SIGNAL(applicationSettingsUpdated()));
	connect(m_view, SIGNAL(applicationAutosaveChanged(bool)), this, SIGNAL(applicationSettingsUpdated()));
	connect(m_view, SIGNAL(applicationAutosaveIntervalChanged(int)), this, SIGNAL(applicationSettingsUpdated()));

	connect(m_view, SIGNAL(scenarioEditPageViewChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditSpellCheckChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditSpellCheckLanguageChanged(int)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditTextColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditBackgroundColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditNonprintableTextColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditFolderTextColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditFolderBackgroundColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditCurrentStyleChanged(QString)), this, SIGNAL(scenarioEditSettingsUpdated()));

	connect(m_view, SIGNAL(navigatorShowScenesNumbersChanged(bool)), this, SIGNAL(navigatorSettingsUpdated()));

	connect(m_view, SIGNAL(navigatorShowScenesTextChanged(bool)), this, SIGNAL(navigatorSettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCurrentTypeChanged()), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryPagesSecondsChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersCharactersChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersSecondsChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersConsiderSpaces(bool)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphActionChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50ActionChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphDialogChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50DialogChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));

	//
	// Библиотека стилей
	//
	connect(m_view, SIGNAL(styleLibraryNewPressed()), this, SLOT(styleLibraryNewPressed()));
	connect(m_view, SIGNAL(styleLibraryEditPressed(QModelIndex)), this, SLOT(styleLibraryEditPressed(QModelIndex)));
	connect(m_view, SIGNAL(styleLibraryRemovePressed(QModelIndex)), this, SLOT(styleLibraryRemovePressed(QModelIndex)));
	connect(m_view, SIGNAL(styleLibraryLoadPressed()), this, SLOT(styleLibraryLoadPressed()));
	connect(m_view, SIGNAL(styleLibrarySavePressed(QModelIndex)), this, SLOT(styleLibrarySavePressed(QModelIndex)));
}
