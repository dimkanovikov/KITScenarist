#include <QApplication>

#include <QFontDatabase>

#include <UserInterface/widget.h>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//
	// Загрузим шрифт Courier New в базу шрифтов программы, если его там ещё нет
	//
	QFontDatabase fontDatabase;
	fontDatabase.addApplicationFont(":/Fonts/Courier New");

	Widget w;
	w.exec();

	return a.exec();
}
