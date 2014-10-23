#include "Application.h"

#include <ManagementLayer/ApplicationManager.h>

int main(int argc, char *argv[])
{
    Application application(argc, argv);

    //
    // Получим имя файла, который пользователь возможно хочет открыть
    //
    QString fileToOpen = application.arguments().value(1, QString::null);
    ManagementLayer::ApplicationManager applicationManager;
    applicationManager.exec(fileToOpen);

    //
    // Установим управляющего в приложение, для возможности открытия файлов
    //
    application.setupManager(&applicationManager);

	return application.exec();
}
