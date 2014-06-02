#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFontDatabase>
#include <QTranslator>

#include <ManagementLayer/ApplicationManager.h>
#include <QFontMetricsF>
#include <QDebug>


int main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	//
	// Настроим информацию о приложении
	//
	application.setOrganizationName("DimkaNovikov labs.");
	application.setOrganizationDomain("dimkanovikov.pro");
	application.setApplicationName("Scenarist");
	application.setApplicationVersion("0.2.4");

	//
	// Настроим стиль отображения внешнего вида приложения
	//
    application.setStyle(QStyleFactory::create("Fusion"));
	//
	// FIXME: почему-то ссылки не меняют цвет, если обновлять палитру на лету,
	// поэтому устанавливаю при загрузке приложения такой цвет, который подходит
	// к обеим темам
	//
	QPalette palette = QApplication::style()->standardPalette();
	palette.setColor(QPalette::Link, QColor("#0C6ACC"));
    application.setPalette(palette);

	//
	// Загрузим Courier New в базу шрифтов программы, если его там ещё нет
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

    //
    // Получим имя файла, который пользователь возможно хочет открыть
    //
    QString fileToOpen = application.arguments().value(1, QString::null);

	ManagementLayer::ApplicationManager applicationManager;
	applicationManager.exec(fileToOpen);

	return application.exec();
}
