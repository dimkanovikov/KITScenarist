#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFontDatabase>
#include <QTranslator>

#include <ManagementLayer/ApplicationManager.h>

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	//
	// Настроим информацию о приложении
	//
	application.setOrganizationName("DimkaNovikov labs.");
	application.setOrganizationDomain("dimkanovikov.pro");
	application.setApplicationName("Scenarist");
	application.setApplicationVersion("0.0.1");

	//
	// Настроим стиль отображения внешнего вида приложения
	//
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setPalette(QApplication::style()->standardPalette());

	//
	// Загрузим шрифт Courier New в базу шрифтов программы, если его там ещё нет
	//
	QFontDatabase fontDatabase;
	fontDatabase.addApplicationFont(":/Fonts/Courier New");

    //
    // Подключим файл переводов Qt
    //
    QTranslator* russianQtTranslator = new QTranslator;
    russianQtTranslator->load(":/Translations/Translations/qt_ru.qm");
    application.installTranslator(russianQtTranslator);

    //
    // Подключим дополнительный файл переводов Qt
    //
    QTranslator* russianQtBaseTranslator = new QTranslator;
    russianQtBaseTranslator->load(":/Translations/Translations/qtbase_ru.qm");
    application.installTranslator(russianQtBaseTranslator);

	//
	// Подключим файл переводов программы
	//
	QTranslator* russianTranslator = new QTranslator;
	russianTranslator->load(":/Translations/Translations/Scenarist_ru.qm");
    application.installTranslator(russianTranslator);

	ManagementLayer::ApplicationManager applicationManager;
	applicationManager.exec();

	return application.exec();
}
