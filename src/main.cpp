#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFontDatabase>

#include <UserInterface/widget.h>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setPalette(QApplication::style()->standardPalette());

	//
	// Загрузим шрифт Courier New в базу шрифтов программы, если его там ещё нет
	//
	QFontDatabase fontDatabase;
	fontDatabase.addApplicationFont(":/Fonts/Courier New");

	//
	// Настроим версию приложения
	//
	a.setApplicationVersion("0.0.1");

	Widget w;
	w.exec();

	return a.exec();
}
