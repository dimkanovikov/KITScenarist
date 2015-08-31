#include "SettingsManager.h"
#include "SettingsTemplatesManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/Chronometry/PagesChronometer.h>
#include <BusinessLayer/Chronometry/CharactersChronometer.h>
#include <BusinessLayer/Chronometry/ConfigurableChronometer.h>

#include <UserInterfaceLayer/Settings/SettingsView.h>

#include <3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.h>
#include <3rd_party/Widgets/HierarchicalHeaderView/HierarchicalTableModel.h>
#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QApplication>
#include <QFileDialog>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStringListModel>

using ManagementLayer::SettingsManager;
using ManagementLayer::SettingsTemplatesManager;
using BusinessLogic::ScenarioTemplateFacade;
using BusinessLogic::ScenarioBlockStyle;
using UserInterface::SettingsView;

namespace {
	/**
	 * @brief Расширение файла шаблона сценария
	 */
	const QString SCENARIO_TEMPLATE_FILE_EXTENSION = "kitss";

	/**
	 * @brief Сформировать строку таблицы переходов между блоками для заданного типа
	 */
	static QList<QStandardItem*> blocksJumpsModelRow(ScenarioBlockStyle::Type _forType) {

		const QString typeShortName = ScenarioBlockStyle::typeName(_forType);
		const QString shortcut =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("scenario-editor/shortcuts/%1").arg(typeShortName),
					DataStorageLayer::SettingsStorage::ApplicationSettings
					);
		const bool BEAUTIFY_NAME = true;
		int jumpForTab =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("scenario-editor/styles-jumping/from-%1-by-tab").arg(typeShortName),
					DataStorageLayer::SettingsStorage::ApplicationSettings
					).toInt();
		int jumpForEnter =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("scenario-editor/styles-jumping/from-%1-by-enter").arg(typeShortName),
					DataStorageLayer::SettingsStorage::ApplicationSettings
					).toInt();
		int changeForTab =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("scenario-editor/styles-changing/from-%1-by-tab").arg(typeShortName),
					DataStorageLayer::SettingsStorage::ApplicationSettings
					).toInt();
		int changeForEnter =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("scenario-editor/styles-changing/from-%1-by-enter").arg(typeShortName),
					DataStorageLayer::SettingsStorage::ApplicationSettings
					).toInt();

		QList<QStandardItem*> result;
		result << new QStandardItem(ScenarioBlockStyle::typeName(_forType, BEAUTIFY_NAME))
			   << new QStandardItem(shortcut)
			   << new QStandardItem(ScenarioBlockStyle::typeName((ScenarioBlockStyle::Type)jumpForTab, BEAUTIFY_NAME))
			   << new QStandardItem(ScenarioBlockStyle::typeName((ScenarioBlockStyle::Type)jumpForEnter, BEAUTIFY_NAME))
			   << new QStandardItem(ScenarioBlockStyle::typeName((ScenarioBlockStyle::Type)changeForTab, BEAUTIFY_NAME))
			   << new QStandardItem(ScenarioBlockStyle::typeName((ScenarioBlockStyle::Type)changeForEnter, BEAUTIFY_NAME));
		result.first()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		return result;
	}

	/**
	 * @brief Сформировать таблицу переходов между блоками
	 */
	static QAbstractItemModel* blocksJumpsModel(QObject* _parent) {
		const int ROWS_COUNT = 0;
		const int COLUMNS_COUNT = 6;
		QStandardItemModel* blocksJumpsModel = new QStandardItemModel(ROWS_COUNT, COLUMNS_COUNT, _parent);
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::SceneHeading));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::SceneCharacters));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::Action));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::Character));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::Parenthetical));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::Dialogue));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::Transition));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::Note));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::Title));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::NoprintableText));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::SceneGroupHeader));
		blocksJumpsModel->appendRow(::blocksJumpsModelRow(ScenarioBlockStyle::FolderHeader));

		return blocksJumpsModel;
	}
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

void SettingsManager::aboutResetSettings()
{
	ProgressWidget progress(m_view);
	progress.showProgress(tr("Restoring"),
		tr("Please wait. Restoring settings to default values can take few minutes."));

	//
	// Сбрасываем настройки
	//
	DataStorageLayer::StorageFacade::settingsStorage()->resetValues(
		DataStorageLayer::SettingsStorage::ApplicationSettings);

	//
	// Перезагружаем интерфейс
	//
	initView();

	progress.close();
}

void SettingsManager::applicationLanguageChanged(int _value)
{
	storeValue("application/language", _value);

	//
	// Уведомляем о том, что язык сменится после перезапуска
	//
	QLightBoxMessage::information(m_view, QString::null, tr("Language will be change after application restart."));
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

void SettingsManager::applicationSaveBackupsChanged(bool _value)
{
	storeValue("application/save-backups", _value);
}

void SettingsManager::applicationSaveBackupsFolderChanged(const QString& _value)
{
	storeValue("application/save-backups-folder", _value);
}

void SettingsManager::scenarioEditPageViewChanged(bool _value)
{
	storeValue("scenario-editor/page-view", _value);
}

void SettingsManager::scenarioEditShowScenesNumbersChanged(bool _value)
{
	storeValue("scenario-editor/show-scenes-numbers", _value);
}

void SettingsManager::scenarioEditHighlightCurrentLineChanged(bool _value)
{
	storeValue("scenario-editor/highlight-current-line", _value);
}

void SettingsManager::scenarioEditEnableAutoReplacing(bool _value)
{
	storeValue("scenario-editor/auto-replacing", _value);
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

void SettingsManager::scenarioEditTextColorDarkChanged(const QColor&_value)
{
	storeValue("scenario-editor/text-color-dark", _value);
}

void SettingsManager::scenarioEditBackgroundColorDarkChanged(const QColor& _value)
{
	storeValue("scenario-editor/background-color-dark", _value);
}

void SettingsManager::scenarioEditNonprintableTextColorDarkChanged(const QColor& _value)
{
	storeValue("scenario-editor/nonprintable-text-color-dark", _value);
}

void SettingsManager::scenarioEditFolderTextColorDarkChanged(const QColor& _value)
{
	storeValue("scenario-editor/folder-text-color-dark", _value);
}

void SettingsManager::scenarioEditFolderBackgroundColorDarkChanged(const QColor& _value)
{
	storeValue("scenario-editor/folder-background-color-dark", _value);
}

void SettingsManager::scenarioEditCurrentTemplateChanged(const QString& _value)
{
	storeValue("scenario-editor/current-style", _value);
}

void SettingsManager::scenarioEditAutoJumpToNextBlockChanged(bool _value)
{
	storeValue("scenario-editor/auto-styles-jumping", _value);
}

void SettingsManager::scenarioEditBlockSettingsChanged(const QString& _block, const QString& _shortcut, const QString& _jumpTab,
	const QString& _jumpEnter, const QString& _changeTab, const QString& _changeEnter)
{
	//
	// Переведём имена в значения блоков ScenarioBlockStyle::Type
	//
	const bool BEAUTIFY_NAME = true;
	ScenarioBlockStyle::Type block = ScenarioBlockStyle::typeForName(_block, BEAUTIFY_NAME);
	ScenarioBlockStyle::Type jumpTab = ScenarioBlockStyle::typeForName(_jumpTab, BEAUTIFY_NAME);
	ScenarioBlockStyle::Type jumpEnter = ScenarioBlockStyle::typeForName(_jumpEnter, BEAUTIFY_NAME);
	ScenarioBlockStyle::Type changeTab = ScenarioBlockStyle::typeForName(_changeTab, BEAUTIFY_NAME);
	ScenarioBlockStyle::Type changeEnter = ScenarioBlockStyle::typeForName(_changeEnter, BEAUTIFY_NAME);

	//
	// Сохраним параметры
	//
	storeValue(QString("scenario-editor/shortcuts/%1").arg(ScenarioBlockStyle::typeName(block)), _shortcut);
	storeValue(QString("scenario-editor/styles-jumping/from-%1-by-tab").arg(ScenarioBlockStyle::typeName(block)), jumpTab);
	storeValue(QString("scenario-editor/styles-jumping/from-%1-by-enter").arg(ScenarioBlockStyle::typeName(block)), jumpEnter);
	storeValue(QString("scenario-editor/styles-changing/from-%1-by-tab").arg(ScenarioBlockStyle::typeName(block)), changeTab);
	storeValue(QString("scenario-editor/styles-changing/from-%1-by-enter").arg(ScenarioBlockStyle::typeName(block)), changeEnter);
}

void SettingsManager::scenarioEditReviewUseWordHighlightChanged(bool _value)
{
	storeValue("scenario-editor/review/use-highlight", _value);
}

void SettingsManager::navigatorShowScenesNumbersChanged(bool _value)
{
	storeValue("navigator/show-scenes-numbers", _value);
}

void SettingsManager::navigatorShowSceneDescriptionChanged(bool _value)
{
	storeValue("navigator/show-scene-description", _value);
}

void SettingsManager::navigatorSceneDescriptionIsSceneTextChanged(bool _value)
{
	storeValue("navigator/scene-description-is-scene-text", _value);
}

void SettingsManager::navigatorSceneDescriptionHeightChanged(int _value)
{
	storeValue("navigator/scene-description-height", _value);
}

void SettingsManager::chronometryUsedChanged(bool _value)
{
	storeValue("chronometry/used", _value);
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

void SettingsManager::chronometryConfigurableSecondsForParagraphSceneHeadingChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-paragraph/scene_heading", _value);
}

void SettingsManager::chronometryConfigurableSecondsFor50SceneHeadingChanged(double _value)
{
	storeValue("chronometry/configurable/seconds-for-every-50/scene_heading", _value);
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

void SettingsManager::pagesCounterUsedChanged(bool _value)
{
	storeValue("counters/pages/used", _value);
}

void SettingsManager::wordsCounterUsedChanged(bool _value)
{
	storeValue("counters/words/used", _value);
}

void SettingsManager::simbolsCounterUsedChanged(bool _value)
{
	storeValue("counters/simbols/used", _value);
}

void SettingsManager::templateLibraryNewPressed()
{
	SettingsTemplatesManager templatesManager(this, m_view);
	templatesManager.newTemplate();
}

void SettingsManager::templateLibraryEditPressed(const QModelIndex& _templateIndex)
{
	//
	// Определим название шаблона к изменению
	//
	const int TEMPLATE_NAME_COLUMN = 0;
	QModelIndex templateeNameIndex = _templateIndex.sibling(_templateIndex.row(), TEMPLATE_NAME_COLUMN);
	QString templateToEditName = ScenarioTemplateFacade::templatesList()->data(templateeNameIndex).toString();

	//
	// Изменим шаблон
	//
	SettingsTemplatesManager templatesManager(this, m_view);
	bool templateChanged = templatesManager.editTemplate(templateToEditName);
	if (templateChanged) {
		emit scenarioEditSettingsUpdated();
	}
}

void SettingsManager::templateLibraryRemovePressed(const QModelIndex& _templateIndex)
{
	//
	// Определим название шаблона к удалению
	//
	const int TEMPLATE_NAME_COLUMN = 0;
	QModelIndex templateNameIndex = _templateIndex.sibling(_templateIndex.row(), TEMPLATE_NAME_COLUMN);
	QString templateToDeleteName = ScenarioTemplateFacade::templatesList()->data(templateNameIndex).toString();

	//
	// Удалим шаблон
	//
	ScenarioTemplateFacade::removeTemplate(templateToDeleteName);
}

void SettingsManager::templateLibraryLoadPressed()
{
	//
	// Выбрать файл для загрузки
	//
	QString templateFilePath =
			QFileDialog::getOpenFileName(m_view, tr("Choose file to load"), QDir::homePath(),
				tr("Scenario Template Files (*.%1)").arg(SCENARIO_TEMPLATE_FILE_EXTENSION));

	if (!templateFilePath.isEmpty()) {
		//
		// Сохранить шаблон в библиотеке
		//
		ScenarioTemplateFacade::saveTemplate(templateFilePath);
	}
}

void SettingsManager::templateLibrarySavePressed(const QModelIndex& _templateIndex)
{
	//
	// Определим шаблон
	//
	const int TEMPLATE_NAME_COLUMN = 0;
	QModelIndex templateNameIndex = _templateIndex.sibling(_templateIndex.row(), TEMPLATE_NAME_COLUMN);
	QString templateToSaveName = ScenarioTemplateFacade::templatesList()->data(templateNameIndex).toString();

	//
	// Выбрать файл для сохранения
	//
	QString templateFilePath =
			QFileDialog::getSaveFileName(m_view, tr("Choose file to save"), QDir::homePath(),
				tr("Scenario Template Files (*.%1)").arg(SCENARIO_TEMPLATE_FILE_EXTENSION));

	//
	// Сохраним шаблон в файл
	//
	if (!templateFilePath.isEmpty()) {
		if (!templateFilePath.endsWith(SCENARIO_TEMPLATE_FILE_EXTENSION)) {
			templateFilePath += "." + SCENARIO_TEMPLATE_FILE_EXTENSION;
		}
		ScenarioTemplateFacade::getTemplate(templateToSaveName).saveToFile(templateFilePath);
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
	// Загрузить библиотеку шаблонов
	//
	m_view->setTemplatesModel(BusinessLogic::ScenarioTemplateFacade::templatesList());

	//
	// Загрузить настройки
	//

	//
	// Настройки приложения
	//
	m_view->setApplicationLanguage(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/language",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
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
	m_view->setApplicationSaveBackups(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/save-backups",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setApplicationSaveBackupsFolder(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/save-backups-folder",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
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
	m_view->setScenarioEditShowScenesNumbers(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/show-scenes-numbers",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setScenarioEditHighlightCurrentLine(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/highlight-current-line",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setScenarioEditEnableAutoReplacing(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/auto-replacing",
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
	m_view->setScenarioEditTextColorDark(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/text-color-dark",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditBackgroundColorDark(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/background-color-dark",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditNonprintableTexColorDark(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/nonprintable-text-color-dark",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditFolderTextColorDark(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/folder-text-color-dark",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	m_view->setScenarioEditFolderBackgroundColorDark(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/folder-background-color-dark",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);
	// ... текущий шаблон
	m_view->setScenarioEditCurrentTemplate(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/current-style",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
	// ... автоматический переход к следующему блоку
	m_view->setScenarioEditAutoJumpToNextBlock(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/auto-styles-jumping",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	// ... рецензирование
	m_view->setScenarioEditReviewUseWordHighlight(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/review/use-highlight",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	// ... модель переходов между блоками, её заголовок и делегат
	QStandardItemModel* blockJumpsHeaderModel = new QStandardItemModel(this);
	{
		QStandardItem * columnAfterText = new QStandardItem(tr("Press in paragraph's end"));
		{
			columnAfterText->appendColumn(QList<QStandardItem*>() << new QStandardItem(tr("Tab")));
			columnAfterText->appendColumn(QList<QStandardItem*>() << new QStandardItem(tr("Enter")));
		}
		QStandardItem * columnEmptyText = new QStandardItem(tr("Press in empty paragraph"));
		{
			columnEmptyText->appendColumn(QList<QStandardItem*>() << new QStandardItem(tr("Tab")));
			columnEmptyText->appendColumn(QList<QStandardItem*>() << new QStandardItem(tr("Enter")));
		}

		blockJumpsHeaderModel->setItem(0, 0, new QStandardItem(tr("Block Name")));
		blockJumpsHeaderModel->setItem(0, 1, new QStandardItem(tr("Shortcut")));
		blockJumpsHeaderModel->setItem(0, 2, columnAfterText);
		blockJumpsHeaderModel->setItem(0, 3, columnEmptyText);
	}
	HierarchicalTableModel* blocksJumpsModel = new HierarchicalTableModel(this);
	blocksJumpsModel->setSourceModel(::blocksJumpsModel(this));
	blocksJumpsModel->setHorizontalHeaderModel(blockJumpsHeaderModel);
	QStringList delegateModel;
	{
		const bool BEAUTIFY_NAME = true;
		delegateModel << ScenarioBlockStyle::typeName(ScenarioBlockStyle::SceneHeading, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::SceneCharacters, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Action, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Character, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Parenthetical, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Dialogue, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Transition, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Note, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Title, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::NoprintableText, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::SceneGroupHeader, BEAUTIFY_NAME)
					  << ScenarioBlockStyle::typeName(ScenarioBlockStyle::FolderHeader, BEAUTIFY_NAME);
	}
	m_view->setBlocksSettingsModel(blocksJumpsModel, new QStringListModel(delegateModel, this));


	//
	// Настройки навигатора
	//
	m_view->setNavigatorShowScenesNumbers(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scenes-numbers",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setNavigatorShowSceneDescription(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/show-scene-description",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setNavigatorSceneDescriptionIsSceneText(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/scene-description-is-scene-text",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setNavigatorSceneDescriptionHeight(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"navigator/scene-description-height",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);

	//
	// Настройки хронометража
	//
	// ... использование
	m_view->setChronometryUsed(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/used",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
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
	m_view->setChronometryConfigurableSecondsForParagraphSceneHeading(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-paragraph/scene_heading",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_view->setChronometryConfigurableSecondsFor50SceneHeading(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-every-50/scene_heading",
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

	//
	// Настройки счётчиков
	//
	m_view->setPagesCounterUsed(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"counters/pages/used",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setWordsCounterUsed(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"counters/words/used",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_view->setSimbolsCounterUsed(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"counters/simbols/used",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
				);
}

void SettingsManager::initConnections()
{
	//
	// Сбросить настройки
	//
	connect(m_view, SIGNAL(resetSettings()), this, SLOT(aboutResetSettings()));

	//
	// Сохранение изменений параметров
	//
	connect(m_view, SIGNAL(applicationLanguageChanged(int)), this, SLOT(applicationLanguageChanged(int)));
	connect(m_view, SIGNAL(applicationUseDarkThemeChanged(bool)), this, SLOT(applicationUseDarkThemeChanged(bool)));
	connect(m_view, SIGNAL(applicationAutosaveChanged(bool)), this, SLOT(applicationAutosaveChanged(bool)));
	connect(m_view, SIGNAL(applicationAutosaveIntervalChanged(int)), this, SLOT(applicationAutosaveIntervalChanged(int)));
	connect(m_view, SIGNAL(applicationSaveBackupsChanged(bool)), this, SLOT(applicationSaveBackupsChanged(bool)));
	connect(m_view, SIGNAL(applicationSaveBackupsFolderChanged(QString)), this, SLOT(applicationSaveBackupsFolderChanged(QString)));

	connect(m_view, SIGNAL(scenarioEditPageViewChanged(bool)), this, SLOT(scenarioEditPageViewChanged(bool)));
	connect(m_view, SIGNAL(scenarioEditShowScenesNumbersChanged(bool)), this, SLOT(scenarioEditShowScenesNumbersChanged(bool)));
	connect(m_view, SIGNAL(scenarioEditHighlightCurrentLineChanged(bool)), this, SLOT(scenarioEditHighlightCurrentLineChanged(bool)));
	connect(m_view, SIGNAL(scenarioEditEnableAutoReplacing(bool)), this, SLOT(scenarioEditEnableAutoReplacing(bool)));
	connect(m_view, SIGNAL(scenarioEditSpellCheckChanged(bool)), this, SLOT(scenarioEditSpellCheckChanged(bool)));
	connect(m_view, SIGNAL(scenarioEditSpellCheckLanguageChanged(int)), this, SLOT(scenarioEditSpellCheckLanguageChanged(int)));
	connect(m_view, SIGNAL(scenarioEditTextColorChanged(QColor)), this, SLOT(scenarioEditTextColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditBackgroundColorChanged(QColor)), this, SLOT(scenarioEditBackgroundColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditNonprintableTextColorChanged(QColor)), this, SLOT(scenarioEditNonprintableTextColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditFolderTextColorChanged(QColor)), this, SLOT(scenarioEditFolderTextColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditFolderBackgroundColorChanged(QColor)), this, SLOT(scenarioEditFolderBackgroundColorChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditTextColorDarkChanged(QColor)), this, SLOT(scenarioEditTextColorDarkChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditBackgroundColorDarkChanged(QColor)), this, SLOT(scenarioEditBackgroundColorDarkChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditNonprintableTextColorDarkChanged(QColor)), this, SLOT(scenarioEditNonprintableTextColorDarkChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditFolderTextColorDarkChanged(QColor)), this, SLOT(scenarioEditFolderTextColorDarkChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditFolderBackgroundColorDarkChanged(QColor)), this, SLOT(scenarioEditFolderBackgroundColorDarkChanged(QColor)));
	connect(m_view, SIGNAL(scenarioEditCurrentTemplateChanged(QString)), this, SLOT(scenarioEditCurrentTemplateChanged(QString)));
	connect(m_view, SIGNAL(scenarioEditAutoJumpToNextBlockChanged(bool)), this, SLOT(scenarioEditAutoJumpToNextBlockChanged(bool)));
	connect(m_view, SIGNAL(scenarioEditBlockSettingsChanged(QString,QString,QString,QString,QString,QString)),
			this, SLOT(scenarioEditBlockSettingsChanged(QString,QString,QString,QString,QString,QString)));
	connect(m_view, SIGNAL(scenarioEditReviewUseWordHighlightChanged(bool)), this, SLOT(scenarioEditReviewUseWordHighlightChanged(bool)));

	connect(m_view, SIGNAL(navigatorShowScenesNumbersChanged(bool)), this, SLOT(navigatorShowScenesNumbersChanged(bool)));
	connect(m_view, SIGNAL(navigatorShowSceneDescriptionChanged(bool)), this, SLOT(navigatorShowSceneDescriptionChanged(bool)));
	connect(m_view, SIGNAL(navigatorSceneDescriptionIsSceneTextChanged(bool)), this, SLOT(navigatorSceneDescriptionIsSceneTextChanged(bool)));
	connect(m_view, SIGNAL(navigatorSceneDescriptionHeightChanged(int)), this, SLOT(navigatorSceneDescriptionHeightChanged(int)));

	connect(m_view, SIGNAL(chronometryUsedChanged(bool)), this, SLOT(chronometryUsedChanged(bool)));
	connect(m_view, SIGNAL(chronometryCurrentTypeChanged()), this, SLOT(chronometryCurrentTypeChanged()));
	connect(m_view, SIGNAL(chronometryPagesSecondsChanged(int)), this, SLOT(chronometryPagesSecondsChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersCharactersChanged(int)), this, SLOT(chronometryCharactersCharactersChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersSecondsChanged(int)), this, SLOT(chronometryCharactersSecondsChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersConsiderSpaces(bool)), this, SLOT(chronometryCharactersConsiderSpacesChanged(bool)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphSceneHeadingChanged(double)),
			this, SLOT(chronometryConfigurableSecondsForParagraphSceneHeadingChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50SceneHeadingChanged(double)),
			this, SLOT(chronometryConfigurableSecondsFor50SceneHeadingChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphActionChanged(double)),
			this, SLOT(chronometryConfigurableSecondsForParagraphActionChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50ActionChanged(double)),
			this, SLOT(chronometryConfigurableSecondsFor50ActionChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphDialogChanged(double)),
			this, SLOT(chronometryConfigurableSecondsForParagraphDialogChanged(double)));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50DialogChanged(double)),
			this, SLOT(chronometryConfigurableSecondsFor50DialogChanged(double)));

	connect(m_view, SIGNAL(pagesCounterUsedChanged(bool)), this, SLOT(pagesCounterUsedChanged(bool)));
	connect(m_view, SIGNAL(wordsCounterUsedChanged(bool)), this, SLOT(wordsCounterUsedChanged(bool)));
	connect(m_view, SIGNAL(simbolsCounterUsedChanged(bool)), this, SLOT(simbolsCounterUsedChanged(bool)));

	//
	// Уведомления об обновлении секции параметров
	//
	connect(m_view, SIGNAL(applicationUseDarkThemeChanged(bool)), this, SIGNAL(applicationSettingsUpdated()));
	connect(m_view, SIGNAL(applicationAutosaveChanged(bool)), this, SIGNAL(applicationSettingsUpdated()));
	connect(m_view, SIGNAL(applicationAutosaveIntervalChanged(int)), this, SIGNAL(applicationSettingsUpdated()));
	connect(m_view, SIGNAL(applicationSaveBackupsChanged(bool)), this, SIGNAL(applicationSettingsUpdated()));
	connect(m_view, SIGNAL(applicationSaveBackupsFolderChanged(QString)), this, SIGNAL(applicationSettingsUpdated()));

	connect(m_view, SIGNAL(applicationUseDarkThemeChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditPageViewChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditShowScenesNumbersChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditHighlightCurrentLineChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditEnableAutoReplacing(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditSpellCheckChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditSpellCheckLanguageChanged(int)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditTextColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditBackgroundColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditNonprintableTextColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditFolderTextColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditFolderBackgroundColorChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditTextColorDarkChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditBackgroundColorDarkChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditNonprintableTextColorDarkChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditFolderTextColorDarkChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditFolderBackgroundColorDarkChanged(QColor)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditCurrentTemplateChanged(QString)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditBlockSettingsChanged(QString,QString,QString,QString,QString,QString)), this, SIGNAL(scenarioEditSettingsUpdated()));
	connect(m_view, SIGNAL(scenarioEditReviewUseWordHighlightChanged(bool)), this, SIGNAL(scenarioEditSettingsUpdated()));

	connect(m_view, SIGNAL(navigatorShowScenesNumbersChanged(bool)), this, SIGNAL(navigatorSettingsUpdated()));
	connect(m_view, SIGNAL(navigatorShowSceneDescriptionChanged(bool)), this, SIGNAL(navigatorSettingsUpdated()));
	connect(m_view, SIGNAL(navigatorSceneDescriptionIsSceneTextChanged(bool)), this, SIGNAL(navigatorSettingsUpdated()));
	connect(m_view, SIGNAL(navigatorSceneDescriptionHeightChanged(int)), this, SIGNAL(navigatorSettingsUpdated()));

	connect(m_view, SIGNAL(chronometryUsedChanged(bool)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCurrentTypeChanged()), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryPagesSecondsChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersCharactersChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersSecondsChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersConsiderSpaces(bool)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphSceneHeadingChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50SceneHeadingChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphActionChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50ActionChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsForParagraphDialogChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryConfigurableSecondsFor50DialogChanged(double)),
			SIGNAL(chronometrySettingsUpdated()));

	connect(m_view, SIGNAL(pagesCounterUsedChanged(bool)), this, SIGNAL(countersSettingsUpdated()));
	connect(m_view, SIGNAL(wordsCounterUsedChanged(bool)), this, SIGNAL(countersSettingsUpdated()));
	connect(m_view, SIGNAL(simbolsCounterUsedChanged(bool)), this, SIGNAL(countersSettingsUpdated()));

	//
	// Библиотека стилей
	//
	connect(m_view, SIGNAL(templateLibraryNewPressed()), this, SLOT(templateLibraryNewPressed()));
	connect(m_view, SIGNAL(templateLibraryEditPressed(QModelIndex)), this, SLOT(templateLibraryEditPressed(QModelIndex)));
	connect(m_view, SIGNAL(templateLibraryRemovePressed(QModelIndex)), this, SLOT(templateLibraryRemovePressed(QModelIndex)));
	connect(m_view, SIGNAL(templateLibraryLoadPressed()), this, SLOT(templateLibraryLoadPressed()));
	connect(m_view, SIGNAL(templateLibrarySavePressed(QModelIndex)), this, SLOT(templateLibrarySavePressed(QModelIndex)));
}
