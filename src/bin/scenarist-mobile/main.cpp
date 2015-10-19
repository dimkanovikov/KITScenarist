#include "Application.h"

#include <ManagementLayer/ApplicationManager.h>

int main(int argc, char *argv[])
{
	Application application(argc, argv);

	ManagementLayer::ApplicationManager applicationManager;
	applicationManager.exec();

	//
	// Установим управляющего в приложение, для возможности открытия файлов
	//
	application.setupManager(&applicationManager);

	return application.exec();
}
