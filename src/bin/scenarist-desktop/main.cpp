#include "Application.h"

#ifdef Q_OS_WIN
#include <QBreakpadHandler.h>
#include <QDir>
#include <QStandardPaths>
#endif

#include <ManagementLayer/ApplicationManager.h>
#include <ManagementLayer/Onboarding/OnboardingManager.h>

#include <QProcess>
#include <QScreen>
#include <QStringList>


int main(int argc, char *argv[])
{
    Application application(argc, argv);

    //
    // Настроем масштабирование, для 4К
    //
    if (application.primaryScreen()->size().width() > 3800
        && qgetenv("QT_SCALE_FACTOR") != "2") {
        //
        // После того, как было настроено разрешение, нужно перезапустить приложение
        //
        qputenv("QT_SCALE_FACTOR", "2");
        QProcess::startDetached(application.arguments().first(), application.arguments());
        return 0;
    }


#ifdef Q_OS_WIN
	//
	// Настроим отлавливание ошибок
	//
	QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString crashReportsFolderPath = appDataFolderPath + QDir::separator() + "CrashReports";
	QBreakpadInstance.setDumpPath(crashReportsFolderPath);
#endif

    //
    // Запускаем диалог стартовой настройки приложения
    //
    ManagementLayer::OnboardingManager onboardingManager;
    if (onboardingManager.needConfigureApp()) {
        onboardingManager.exec();
        QObject::connect(&onboardingManager, &ManagementLayer::OnboardingManager::translationUpdateRequested, &application, &Application::updateTranslation);
        QObject::connect(&onboardingManager, &ManagementLayer::OnboardingManager::finished, &application, &Application::startApp);
        QObject::connect(&application, &Application::started, &onboardingManager, &ManagementLayer::OnboardingManager::close);
    } else {
        application.startApp();
    }

	return application.exec();
}
