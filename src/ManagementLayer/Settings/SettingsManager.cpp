#include "SettingsManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/Chronometry/PagesChronometer.h>
#include <BusinessLayer/Chronometry/CharactersChronometer.h>
#include <BusinessLayer/Chronometry/ConfigurableChronometer.h>

#include <UserInterfaceLayer/Settings/SettingsView.h>

using ManagementLayer::SettingsManager;
using UserInterface::SettingsView;


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

void SettingsManager::textSpellCheckChanged(bool _value)
{
	storeValue("text-editor/spell-checking", _value);
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

void SettingsManager::initView()
{
	//
	// Загрузить настройки
	//

	//
	// Настройки текстового редактора
	//
	m_view->setTextSpellCheck(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"text-editor/spell-checking",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt()
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
					"chronometry/configurable/seconds-for-for-every-50/dialog",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble()
				);
}

void SettingsManager::initConnections()
{
	//
	// Сохранение изменений параметров
	//
	connect(m_view, SIGNAL(textSpellCheckChanged(bool)), this, SLOT(textSpellCheckChanged(bool)));
	connect(m_view, SIGNAL(navigatorShowScenesNumbersChanged(bool)), this, SLOT(navigatorShowScenesNumbersChanged(bool)));
	connect(m_view, SIGNAL(chronometryCurrentTypeChanged()), this, SLOT(chronometryCurrentTypeChanged()));
	connect(m_view, SIGNAL(chronometryPagesSecondsChanged(int)), this, SLOT(chronometryPagesSecondsChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersCharactersChanged(int)), this, SLOT(chronometryCharactersCharactersChanged(int)));
	connect(m_view, SIGNAL(chronometryCharactersSecondsChanged(int)), this, SLOT(chronometryCharactersSecondsChanged(int)));
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
	connect(m_view, SIGNAL(textSpellCheckChanged(bool)), this, SIGNAL(textEditSettingsUpdated()));
	connect(m_view, SIGNAL(navigatorShowScenesNumbersChanged(bool)), this, SIGNAL(navigatorSettingsUpdated()));
	connect(m_view, SIGNAL(navigatorShowScenesTextChanged(bool)), this, SIGNAL(navigatorSettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCurrentTypeChanged()), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryPagesSecondsChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersCharactersChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
	connect(m_view, SIGNAL(chronometryCharactersSecondsChanged(int)), this, SIGNAL(chronometrySettingsUpdated()));
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
}
