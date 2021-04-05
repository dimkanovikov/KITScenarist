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
    //
    // Если необходимо, включаем масштабирование для экранов с высоким DPI
    //
    if (qgetenv("KIT_USE_HIDPI_SCALING") == "1") {
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }

    Application application(argc, argv);

    auto restartApp = [&application] {
        QStringList arguments = application.arguments();
        arguments.removeFirst();
        return QProcess::startDetached(application.arguments().constFirst(), arguments);
    };

    //
    // Если пользователь хочет работать в режиме масштабирования для экранов с высоким DPI
    //
    if (application.shouldUseHidpiScaling()) {
        if (qgetenv("KIT_USE_HIDPI_SCALING") != "1"
            && qputenv("KIT_USE_HIDPI_SCALING", "1")
            && restartApp()) {
            return 0;
        }
    }
    //
    // Настроем масштабирование, для 4К
    //
    else if (application.primaryScreen()->size().width() > 3800
             && qgetenv("QT_SCALE_FACTOR") != "2") {
        //
        // После того, как было настроено разрешение, нужно перезапустить приложение
        //
        if (qputenv("QT_SCALE_FACTOR", "2")
            && restartApp()) {
            return 0;
        }
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
