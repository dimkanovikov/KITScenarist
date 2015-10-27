#include "Application.h"

#include <ManagementLayer/ApplicationManager.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFontDatabase>
#include <QTranslator>


Application::Application(int& _argc, char** _argv) :
	QApplication(_argc, _argv),
	m_applicationManager(0)
{
	//
	// Настроим информацию о приложении
	//
	setOrganizationName("DimkaNovikov labs.");
	setOrganizationDomain("dimkanovikov.pro");
	setApplicationName("Scenarist");
	setApplicationVersion("0.0.1");

	//
	// Загрузим Courier New в базу шрифтов программы, если его там ещё нет
	//
	QFontDatabase fontDatabase;
	fontDatabase.addApplicationFont(":/Fonts/Courier New");

	//
	// Настроим перевод приложения
	//
	initTranslation();

    //
    // Настроим шрифт
    //
#ifdef Q_OS_ANDROID
    setFont(QFont("Roboto", 14));
#endif
}

void Application::setupManager(ManagementLayer::ApplicationManager *_manager)
{
	m_applicationManager = _manager;
}

void Application::initTranslation()
{
	//
	// Определим язык перевода
	//
	const int language =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/language",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	QString translationSuffix = QLocale::system().name();
	translationSuffix.truncate(translationSuffix.lastIndexOf('_'));
	//
	// ... если не удалось определить локаль, используем англоязычный перевод
	//
	QLocale::Language currentLanguage = QLocale::AnyLanguage;
	if (translationSuffix.isEmpty()) {
		translationSuffix = "en";
		currentLanguage = QLocale::English;
	}

	if (language == 0) {
		translationSuffix = "ru";
		currentLanguage = QLocale::Russian;
	} else if (language == 1) {
		translationSuffix = "es";
		currentLanguage = QLocale::Spanish;
	} else if (language == 2) {
		translationSuffix = "en";
		currentLanguage = QLocale::English;
	}

	QLocale::setDefault(QLocale(currentLanguage));

	//
	// Для отличных от английского, подключаем переводы самой Qt
	//
	if (translationSuffix != "en") {
		//
		// Подключим файл переводов Qt
		//
		QTranslator* qtTranslator = new QTranslator;
		qtTranslator->load(":/Translations/Translations/qt_" + translationSuffix + ".qm");
		installTranslator(qtTranslator);

		//
		// Подключим дополнительный файл переводов Qt
		//
		QTranslator* qtBaseTranslator = new QTranslator;
		qtBaseTranslator->load(":/Translations/Translations/qtbase_" + translationSuffix + ".qm");
		installTranslator(qtBaseTranslator);
	}

	//
	// Подключим файл переводов программы
	//
	QTranslator* appTranslator = new QTranslator;
	appTranslator->load(":/Translations/Translations/Scenarist_" + translationSuffix + ".qm");
	installTranslator(appTranslator);
}
