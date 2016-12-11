#include "Application.h"

#ifdef Q_OS_WIN
#include <QBreakpadHandler.h>
#include <QDir>
#include <QStandardPaths>
#endif

#include <ManagementLayer/ApplicationManager.h>


int main(int argc, char *argv[])
{
	Application application(argc, argv);

#ifdef Q_OS_WIN
	//
	// Настроим отлавливание ошибок
	//
	QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString crashReportsFolderPath = appDataFolderPath + QDir::separator() + "CrashReports";
	QBreakpadInstance.setDumpPath(crashReportsFolderPath);
#endif

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
