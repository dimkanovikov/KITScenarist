#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFontDatabase>

#include <ManagementLayer/ApplicationManager.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//
	// Настроим информацию о приложении
	//
	a.setOrganizationName("DimkaNovikov labs.");
	a.setOrganizationDomain("dimkanovikov.pro");
	a.setApplicationName("Scenarist");
	a.setApplicationVersion("0.0.1");

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

	ApplicationManager m;
	m.exec();

	return a.exec();
}
