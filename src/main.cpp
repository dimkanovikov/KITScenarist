#include <QApplication>

#include <ManagementLayer/ApplicationManager.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	ApplicationManager m;
	m.exec();

	return a.exec();
}
