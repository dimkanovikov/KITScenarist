#include "Application.h"

#ifdef Q_OS_WIN
#include <QBreakpadHandler.h>
#include <QDir>
#include <QStandardPaths>
#endif

#include <ManagementLayer/ApplicationManager.h>
#include <ManagementLayer/Onboarding/OnboardingManager.h>

#include <QScreen>


int main(int argc, char *argv[])
{
    //
    // Настроем масштабирование, для 4К
    //
    if (QApplication(argc, argv).primaryScreen()->size().width() > 3800) {
        qputenv("QT_SCALE_FACTOR", "2");
    }

    //
    // После того, как было настроено разрешение, можно стартовать основное приложение
    //
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
