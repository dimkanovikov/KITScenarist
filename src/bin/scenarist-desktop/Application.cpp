#include "Application.h"

#include <ManagementLayer/ApplicationManager.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QStyle>
#include <QStyleFactory>
#include <QTranslator>

namespace {
	/**
	 * @brief Подготовить путь к файлу для сохранения
	 */
	static QString preparePath(const QString& _path) {
		QString newPath = _path;
#ifdef Q_OS_MAC
		//
		// Это две разные буквы!
		// Первую даёт нам мак, когда открываешь файл через двойной щелчок на нём
		//
		newPath.replace("й", "й");
#endif
		return newPath;
	}
}


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
	setApplicationVersion("0.5.9 beta 3");

	//
	// Настроим стиль отображения внешнего вида приложения
	//
	setStyle(QStyleFactory::create("Fusion"));

	//
	// Загрузим Courier New в базу шрифтов программы, если его там ещё нет
	//
	QFontDatabase fontDatabase;
	fontDatabase.addApplicationFont(":/Fonts/Courier New");

	//
	// Настроим перевод приложения
	//
	initTranslation();
}

void Application::setupManager(ManagementLayer::ApplicationManager *_manager)
{
	m_applicationManager = _manager;
}

bool Application::event(QEvent *_event)
{
	bool result = true;
	if (_event->type() == QEvent::FileOpen
		&& m_applicationManager != 0) {
		QFileOpenEvent* fileOpenEvent = static_cast<QFileOpenEvent*>(_event);
		m_applicationManager->openFile(::preparePath(fileOpenEvent->file()));
	} else {
		result = QApplication::event(_event);
	}

	return result;
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
	} else if (language == 3) {
		translationSuffix = "fr";
		currentLanguage = QLocale::French;
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
