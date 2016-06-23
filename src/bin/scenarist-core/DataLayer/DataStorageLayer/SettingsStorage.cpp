#include "SettingsStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/SettingsMapper.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <3rd_party/Helpers/ShortcutHelper.h>

#include <QApplication>
#include <QHeaderView>
#include <QSettings>
#include <QSplitter>
#include <QStandardPaths>
#include <QStringList>
#include <QTableView>
#include <QToolButton>
#include <QTreeView>
#include <QUuid>
#include <QWidget>

using namespace DataStorageLayer;
using namespace DataMappingLayer;
using namespace BusinessLogic;

namespace {
	/**
	 * @brief Ключ для хранения положения и размеров интерфейса
	 */
	const QString STATE_AND_GEOMETRY_KEY = "state_and_geometry";

	/**
	 * @brief Имя пользователя из системы
	 */
	static QString systemUserName() {
		QString name = qgetenv("USER");
		if (name.isEmpty()) {
			//
			// Windows
			//
			name = QString::fromLocal8Bit(qgetenv("USERNAME"));
			if (name.isEmpty()) {
				name = "user";
			}
		}
		return name;
	}
}


void SettingsStorage::setValue(const QString& _key, const QString& _value, SettingsPlace _settingsPlace)
{
	//
	// Кэшируем значение
	//
	cacheValue(_key, _value, _settingsPlace);

	//
	// Сохраняем его в заданное хранилище
	//
	if (_settingsPlace == ApplicationSettings) {
		QSettings().setValue(_key.toUtf8().toHex(), _value);
	} else {
		MapperFacade::settingsMapper()->setValue(_key, _value);
	}
}

void SettingsStorage::setValues(const QMap<QString, QString>& _values, const QString& _valuesGroup, SettingsStorage::SettingsPlace _settingsPlace)
{
	//
	// Кэшируем значение
	//
	cacheValue(_valuesGroup, QVariant::fromValue<QMap<QString, QString> >(_values), _settingsPlace);

	//
	// Сохраняем его в заданное хранилище
	//
	if (_settingsPlace == ApplicationSettings) {
		QSettings settings;

		//
		// Очистим группу
		//
		{
			settings.beginGroup(_valuesGroup);
			settings.remove("");
			settings.endGroup();
		}

		//
		// Откроем группу
		//
		settings.beginGroup(_valuesGroup);

		//
		// Сохраним значения
		//
		foreach (const QString& key, _values.keys()) {
			settings.setValue(key.toUtf8().toHex(), _values.value(key));
			m_cachedValuesApp.insert(key, _values.value(key));
		}

		//
		// Закроем группу
		//
		settings.endGroup();
	}
	//
	// В базу данных карта параметров не умеет сохраняться
	//
	else {
		Q_ASSERT_X(0, Q_FUNC_INFO, "Database settings can't save group of settings");
	}

}

QString SettingsStorage::value(const QString& _key, SettingsPlace _settingsPlace, const QString& _defaultValue)
{
	//
	// Пробуем получить значение из кэша
	//
	bool hasCachedValue = false;
	QString value = getCachedValue(_key, _settingsPlace, hasCachedValue).toString();

	//
	// Если в кэше нет, то загружаем из указанного места
	//
	if (!hasCachedValue) {
		if (_settingsPlace == ApplicationSettings) {
			value = QSettings().value(_key.toUtf8().toHex(), QVariant()).toString();
		} else {
			value = MapperFacade::settingsMapper()->value(_key);
		}

		//
		// Если параметр не задан, то используем значение по умолчанию
		//
		if (value.isEmpty()) {
			if (_defaultValue.isEmpty()) {
				value = m_defaultValues.value(_key);
			} else {
				value = _defaultValue;
			}
		}

		//
		// Сохраняем значение в кэш
		//
		cacheValue(_key, value, _settingsPlace);
	}

	return value;
}

QMap<QString, QString> SettingsStorage::values(const QString& _valuesGroup, SettingsStorage::SettingsPlace _settingsPlace)
{
	//
	// Пробуем получить значение из кэша
	//
	bool hasCachedValue = false;
	QMap<QString, QString> settingsValues =
		getCachedValue(_valuesGroup, _settingsPlace, hasCachedValue).value<QMap<QString, QString> >();

	//
	// Если в кэше нет, то загружаем из указанного места
	//
	if (!hasCachedValue) {
		if (_settingsPlace == ApplicationSettings) {
			QSettings settings;

			//
			// Откроем группу для считывания
			//
			settings.beginGroup(_valuesGroup);

			//
			// Получим все ключи
			//
			QStringList keys = settings.childKeys();

			//
			// Получим все значения
			//
			foreach (QString key, keys) {
				 settingsValues.insert(QByteArray::fromHex(key.toUtf8()), settings.value(key).toString());
			}

			//
			// Закроем группу
			//
			settings.endGroup();
		}
		//
		// Из базы данных карта параметров не умеет загружаться
		//
		else {
			Q_ASSERT_X(0, Q_FUNC_INFO, "Database settings can't load group of settings");
		}

		//
		// Сохраняем значение в кэш
		//
		cacheValue(_valuesGroup, QVariant::fromValue<QMap<QString, QString> >(settingsValues), _settingsPlace);
	}

	return settingsValues;
}

void SettingsStorage::resetValues(SettingsStorage::SettingsPlace _settingsPlace)
{
	if (_settingsPlace == ApplicationSettings) {
		//
		// Сбрасываем кэш
		//
		m_cachedValuesApp.clear();

		//
		// Восстанавливаем значения по умолчанию
		//
		foreach (const QString& key, m_defaultValues.keys()) {
			setValue(key, m_defaultValues.value(key), _settingsPlace);
			QApplication::processEvents();
		}
	}
	else {
		Q_ASSERT_X(0, Q_FUNC_INFO, "Can't reset settings stored in database.");
	}
}

void SettingsStorage::saveApplicationStateAndGeometry(QWidget* _widget)
{
	QSettings settings;

	settings.beginGroup(STATE_AND_GEOMETRY_KEY);

	settings.setValue("geometry", _widget->saveGeometry());

	settings.beginGroup("toolbuttons");
	foreach (QToolButton* toolButton, _widget->findChildren<QToolButton*>()) {
		settings.setValue(toolButton->objectName() + "-checked", toolButton->isChecked());
	}
	settings.endGroup();

	settings.beginGroup("splitters");
	foreach (QSplitter* splitter, _widget->findChildren<QSplitter*>()) {
		settings.beginGroup(splitter->objectName());
		settings.setValue("state", splitter->saveState());
		settings.setValue("geometry", splitter->saveGeometry());
		//
		// Сохраняем расположение панелей
		//
		settings.beginGroup("splitter-widgets");
		for (int widgetPos = 0; widgetPos < splitter->count(); ++widgetPos) {
			settings.setValue(splitter->widget(widgetPos)->objectName(), widgetPos);
		}
		settings.endGroup(); // splitter-widgets
		settings.endGroup(); // splitter->objectName()
	}
	settings.endGroup();

	settings.beginGroup("headers");
	foreach (QTableView* table, _widget->findChildren<QTableView*>()) {
		settings.setValue(table->objectName() + "-state", table->horizontalHeader()->saveState());
		settings.setValue(table->objectName() + "-geometry", table->horizontalHeader()->saveGeometry());
	}
	foreach (QTreeView* tree, QApplication::activeWindow()->findChildren<QTreeView*>()) {
		settings.setValue(tree->objectName() + "-state", tree->header()->saveState());
		settings.setValue(tree->objectName() + "-geometry", tree->header()->saveGeometry());
	}
	settings.endGroup();
	settings.endGroup(); // STATE_AND_GEOMETRY_KEY
}

void SettingsStorage::loadApplicationStateAndGeometry(QWidget* _widget)
{
	QSettings settings;

	settings.beginGroup(STATE_AND_GEOMETRY_KEY);

	_widget->restoreGeometry(settings.value("geometry").toByteArray());


	settings.beginGroup("toolbuttons");
	foreach (QToolButton* toolButton, _widget->findChildren<QToolButton*>()) {
		toolButton->setChecked(settings.value(toolButton->objectName() + "-checked", false).toBool());
	}
	settings.endGroup();

	settings.beginGroup("splitters");
	foreach (QSplitter* splitter, _widget->findChildren<QSplitter*>()) {
		settings.beginGroup(splitter->objectName());
		//
		// Восстанавливаем расположение панелей
		//
		settings.beginGroup("splitter-widgets");
		//
		// ... сформируем карту позиционирования
		//
		QMap<int, QWidget*> splitterWidgets;
		for (int widgetPos = 0; widgetPos < splitter->count(); ++widgetPos) {
			QWidget* widget = splitter->widget(widgetPos);
			if (!settings.value(widget->objectName()).isNull()) {
				const int position = settings.value(widget->objectName()).toInt();
				splitterWidgets.insert(position, widget);
			}
		}
		//
		// ... позиционируем сами виджеты
		//
		foreach (int position, splitterWidgets.keys()) {
			splitter->insertWidget(position, splitterWidgets.value(position));
		}
		settings.endGroup(); // splitter-widgets

		//
		// Восстанавливаем состояние и геометрию разделителя
		//
		// Это необходимо делать только после восстановления расположения панелей, т.к. некоторые
		// панели имеют определённый минимальный размер, соответственно этот размер задаётся и
		// области разделителя, в которой он находится
		//
		splitter->restoreState(settings.value("state").toByteArray());
		splitter->restoreGeometry(settings.value("geometry").toByteArray());
		settings.endGroup(); // splitter->objectName()
	}
	settings.endGroup();

	settings.beginGroup("headers");
	foreach (QTableView* table, _widget->findChildren<QTableView*>()) {
		table->horizontalHeader()->restoreState(settings.value(table->objectName() + "-state").toByteArray());
		table->horizontalHeader()->restoreGeometry(settings.value(table->objectName() + "-geometry").toByteArray());
	}
	foreach (QTreeView* tree, _widget->findChildren<QTreeView*>()) {
		tree->header()->restoreState(settings.value(tree->objectName() + "-state").toByteArray());
		tree->header()->restoreGeometry(settings.value(tree->objectName() + "-geometry").toByteArray());
	}
	settings.endGroup();
	settings.endGroup(); // STATE_AND_GEOMETRY_KEY
}

SettingsStorage::SettingsStorage()
{
	//
	// Настроим значения параметров по умолчанию
	//
	m_defaultValues.insert("application/uuid", QUuid::createUuid().toString());
	m_defaultValues.insert("application/language", "-1");
	m_defaultValues.insert("application/user-name", ::systemUserName());
	m_defaultValues.insert("application/use-dark-theme", "0");
	m_defaultValues.insert("application/autosave", "1");
	m_defaultValues.insert("application/autosave-interval", "5");
	m_defaultValues.insert("application/save-backups", "0");
	m_defaultValues.insert("application/save-backups-folder",
		QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
	m_defaultValues.insert("application/modules/research", "1");
	m_defaultValues.insert("application/modules/scenario", "1");
	m_defaultValues.insert("application/modules/characters", "1");
	m_defaultValues.insert("application/modules/locations", "1");
	m_defaultValues.insert("application/modules/statistics", "1");

	m_defaultValues.insert("navigator/show-scenes-numbers", "1");
	m_defaultValues.insert("navigator/show-scene-description", "1");
	m_defaultValues.insert("navigator/scene-description-is-scene-text", "1");
	m_defaultValues.insert("navigator/scene-description-height", "1");

	m_defaultValues.insert("scenario-editor/page-view",
#ifndef MOBILE_OS
						   "1"
#else
						   "0"
#endif
						   );
	m_defaultValues.insert("scenario-editor/zoom-range", "1");
	m_defaultValues.insert("scenario-editor/show-scenes-numbers", "1");
	m_defaultValues.insert("scenario-editor/capitalize-first-word", "1");
	m_defaultValues.insert("scenario-editor/correct-double-capitals", "1");
	m_defaultValues.insert("scenario-editor/replace-three-dots", "1");
	m_defaultValues.insert("scenario-editor/smart-quotes", "1");
	m_defaultValues.insert("scenario-editor/spell-checking", "0");
	m_defaultValues.insert("scenario-editor/text-color", "#000000");
	m_defaultValues.insert("scenario-editor/background-color", "#FEFEFE");
	m_defaultValues.insert("scenario-editor/nonprintable-text-color", "#0AC139");
	m_defaultValues.insert("scenario-editor/folder-text-color", "#FEFEFE");
	m_defaultValues.insert("scenario-editor/folder-background-color", "#CAC6C3");
	m_defaultValues.insert("scenario-editor/text-color-dark", "#EBEBEB");
	m_defaultValues.insert("scenario-editor/background-color-dark", "#3D3D3D");
	m_defaultValues.insert("scenario-editor/nonprintable-text-color-dark", "#0AC139");
	m_defaultValues.insert("scenario-editor/folder-text-color-dark", "#EBEBEB");
	m_defaultValues.insert("scenario-editor/folder-background-color-dark", "#8D2DC4");
	m_defaultValues.insert("scenario-editor/zoom-range", "0");
	//
	m_defaultValues.insert("scenario-editor/shortcuts/scene_heading", ShortcutHelper::makeShortcut("Ctrl+Return"));
	m_defaultValues.insert("scenario-editor/shortcuts/scene_characters", ShortcutHelper::makeShortcut("Ctrl+E"));
	m_defaultValues.insert("scenario-editor/shortcuts/action", ShortcutHelper::makeShortcut("Ctrl+J"));
	m_defaultValues.insert("scenario-editor/shortcuts/character", ShortcutHelper::makeShortcut("Ctrl+U"));
	m_defaultValues.insert("scenario-editor/shortcuts/dialog", ShortcutHelper::makeShortcut("Ctrl+L"));
	m_defaultValues.insert("scenario-editor/shortcuts/parenthetical", ShortcutHelper::makeShortcut("Ctrl+H"));
	m_defaultValues.insert("scenario-editor/shortcuts/transition", ShortcutHelper::makeShortcut("Ctrl+G"));
	m_defaultValues.insert("scenario-editor/shortcuts/note", ShortcutHelper::makeShortcut("Ctrl+P"));
	m_defaultValues.insert("scenario-editor/shortcuts/title", ShortcutHelper::makeShortcut("Ctrl+N"));
	m_defaultValues.insert("scenario-editor/shortcuts/noprintable_text", ShortcutHelper::makeShortcut("Ctrl+Esc"));
	m_defaultValues.insert("scenario-editor/shortcuts/scene_group_header", ShortcutHelper::makeShortcut("Ctrl+D"));
	m_defaultValues.insert("scenario-editor/shortcuts/folder_header", ShortcutHelper::makeShortcut("Ctrl+Space"));
	//
	m_defaultValues.insert("scenario-editor/auto-styles-jumping", "1");
	m_defaultValues.insert("scenario-editor/show-suggestions-in-empty-blocks", "1");
	//
	m_defaultValues.insert("scenario-editor/styles-jumping/from-scene_heading-by-tab", QString::number(ScenarioBlockStyle::SceneCharacters));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-scene_heading-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-scene_characters-by-tab", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-scene_characters-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-action-by-tab", QString::number(ScenarioBlockStyle::Character));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-action-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-character-by-tab", QString::number(ScenarioBlockStyle::Parenthetical));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-character-by-enter", QString::number(ScenarioBlockStyle::Dialogue));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-dialog-by-tab", QString::number(ScenarioBlockStyle::Parenthetical));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-dialog-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-parenthetical-by-tab", QString::number(ScenarioBlockStyle::Dialogue));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-parenthetical-by-enter", QString::number(ScenarioBlockStyle::Dialogue));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-transition-by-tab", QString::number(ScenarioBlockStyle::SceneHeading));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-transition-by-enter", QString::number(ScenarioBlockStyle::SceneHeading));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-note-by-tab", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-note-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-title-by-tab", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-title-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-noprintable_text-by-tab", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-noprintable_text-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-scene_group_header-by-tab", QString::number(ScenarioBlockStyle::SceneHeading));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-scene_group_header-by-enter", QString::number(ScenarioBlockStyle::SceneHeading));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-folder_header-by-tab", QString::number(ScenarioBlockStyle::SceneHeading));
	m_defaultValues.insert("scenario-editor/styles-jumping/from-folder_header-by-enter", QString::number(ScenarioBlockStyle::SceneHeading));
	//
	m_defaultValues.insert("scenario-editor/styles-changing/from-scene_heading-by-tab", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-changing/from-scene_heading-by-enter", QString::number(ScenarioBlockStyle::SceneHeading));
	m_defaultValues.insert("scenario-editor/styles-changing/from-scene_characters-by-tab", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-changing/from-scene_characters-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-changing/from-action-by-tab", QString::number(ScenarioBlockStyle::Character));
	m_defaultValues.insert("scenario-editor/styles-changing/from-action-by-enter", QString::number(ScenarioBlockStyle::SceneHeading));
	m_defaultValues.insert("scenario-editor/styles-changing/from-character-by-tab", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-changing/from-character-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-changing/from-dialog-by-tab", QString::number(ScenarioBlockStyle::Parenthetical));
	m_defaultValues.insert("scenario-editor/styles-changing/from-dialog-by-enter", QString::number(ScenarioBlockStyle::Action));
	m_defaultValues.insert("scenario-editor/styles-changing/from-parenthetical-by-tab", QString::number(ScenarioBlockStyle::Dialogue));
	m_defaultValues.insert("scenario-editor/styles-changing/from-parenthetical-by-enter", QString::number(ScenarioBlockStyle::Parenthetical));
	m_defaultValues.insert("scenario-editor/styles-changing/from-transition-by-tab", QString::number(ScenarioBlockStyle::Transition));
	m_defaultValues.insert("scenario-editor/styles-changing/from-transition-by-enter", QString::number(ScenarioBlockStyle::Transition));
	m_defaultValues.insert("scenario-editor/styles-changing/from-note-by-tab", QString::number(ScenarioBlockStyle::Note));
	m_defaultValues.insert("scenario-editor/styles-changing/from-note-by-enter", QString::number(ScenarioBlockStyle::Note));
	m_defaultValues.insert("scenario-editor/styles-changing/from-title-by-tab", QString::number(ScenarioBlockStyle::Title));
	m_defaultValues.insert("scenario-editor/styles-changing/from-title-by-enter", QString::number(ScenarioBlockStyle::Title));
	m_defaultValues.insert("scenario-editor/styles-changing/from-noprintable_text-by-tab", QString::number(ScenarioBlockStyle::NoprintableText));
	m_defaultValues.insert("scenario-editor/styles-changing/from-noprintable_text-by-enter", QString::number(ScenarioBlockStyle::NoprintableText));
	m_defaultValues.insert("scenario-editor/styles-changing/from-scene_group_header-by-tab", QString::number(ScenarioBlockStyle::SceneGroupHeader));
	m_defaultValues.insert("scenario-editor/styles-changing/from-scene_group_header-by-enter", QString::number(ScenarioBlockStyle::SceneGroupHeader));
	m_defaultValues.insert("scenario-editor/styles-changing/from-folder_header-by-tab", QString::number(ScenarioBlockStyle::FolderHeader));
	m_defaultValues.insert("scenario-editor/styles-changing/from-folder_header-by-enter", QString::number(ScenarioBlockStyle::FolderHeader));
	//
	m_defaultValues.insert("scenario-editor/review/use-highlight", "1");

	m_defaultValues.insert("chronometry/used", "1");
	m_defaultValues.insert("chronometry/current-chronometer-type", "pages-chronometer");
	m_defaultValues.insert("chronometry/pages/seconds", "60");
	m_defaultValues.insert("chronometry/characters/characters", "1000");
	m_defaultValues.insert("chronometry/characters/seconds", "60");
	m_defaultValues.insert("chronometry/configurable/seconds-for-paragraph/scene_heading", "2");
	m_defaultValues.insert("chronometry/configurable/seconds-for-every-50/scene_heading", "0");
	m_defaultValues.insert("chronometry/configurable/seconds-for-paragraph/action", "1");
	m_defaultValues.insert("chronometry/configurable/seconds-for-every-50/action", "1.5");
	m_defaultValues.insert("chronometry/configurable/seconds-for-paragraph/dialog", "2");
	m_defaultValues.insert("chronometry/configurable/seconds-for-every-50/dialog", "2.4");


	m_defaultValues.insert("counters/pages/used", "0");
	m_defaultValues.insert("counters/words/used", "0");
	m_defaultValues.insert("counters/simbols/used", "0");
}

QVariant SettingsStorage::getCachedValue(const QString& _key, SettingsStorage::SettingsPlace _settingsPlace, bool& _ok)
{
	QVariant result;
	if (_settingsPlace == SettingsStorage::ApplicationSettings) {
		_ok = m_cachedValuesApp.contains(_key);
		result = m_cachedValuesApp.value(_key);
	} else {
		_ok = m_cachedValuesDb.contains(_key);
		result = m_cachedValuesDb.value(_key);
	}
	return result;
}

void SettingsStorage::cacheValue(const QString& _key, const QVariant& _value, SettingsStorage::SettingsPlace _settingsPlace)
{
	if (_settingsPlace == SettingsStorage::ApplicationSettings) {
		m_cachedValuesApp.insert(_key, _value);
	} else {
		m_cachedValuesDb.insert(_key, _value);
	}
}
