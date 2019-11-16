#include "Application.h"

#include <ManagementLayer/ApplicationManager.h>

#include <NetworkRequest.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/StyleHelper.h>

#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QStyle>
#include <QStyleFactory>
#include <QTranslator>

namespace {
    /**
     * @brief Подготовить путь к файлу для сохранения
     */
    static QString preparePath(const QString& _path) {
        QString newPath = _path;
#ifdef Q_OS_MAC
        //
        // Это две разные буквы!
        // Первую даёт нам мак, когда открываешь файл через двойной щелчок на нём
        //
        newPath.replace("й", "й");
#endif
        return newPath;
    }
}


Application::Application(int& _argc, char** _argv) :
    QApplication(_argc, _argv)
{
    //
    // Настроим информацию о приложении
    //
    setOrganizationName("DimkaNovikov labs.");
    setOrganizationDomain("dimkanovikov.pro");
    setApplicationName("Scenarist");
    setApplicationVersion("0.7.2 rc 9e");

    //
    // Настроим стиль отображения внешнего вида приложения
    //
    setStyle(new BigMenuIconStyle(QStyleFactory::create("Fusion")));

    //
    // Загрузим шрифты в базу шрифтов программы, если их там ещё нет
    //
    QFontDatabase fontDatabase;
    fontDatabase.addApplicationFont(":/Fonts/Roboto Thin");
    fontDatabase.addApplicationFont(":/Fonts/Roboto Regular");
    fontDatabase.addApplicationFont(":/Fonts/Roboto Medium");
    fontDatabase.addApplicationFont(":/Fonts/Roboto Bold");
    fontDatabase.addApplicationFont(":/Fonts/Roboto Black");
    fontDatabase.addApplicationFont(":/Fonts/Courier New");
    fontDatabase.addApplicationFont(":/Fonts/Courier New Bold");
    fontDatabase.addApplicationFont(":/Fonts/Courier New Italic");
    fontDatabase.addApplicationFont(":/Fonts/Courier New Bold Italic");
    fontDatabase.addApplicationFont(":/Fonts/Courier Prime");
    fontDatabase.addApplicationFont(":/Fonts/Courier Prime Bold");
    fontDatabase.addApplicationFont(":/Fonts/Courier Prime Italic");
    fontDatabase.addApplicationFont(":/Fonts/Courier Prime Bold Italic");

    //
    // Настроим перевод приложения
    //
    updateTranslation();

    //
    // Настроим таймер определения простоя приложения
    //
    m_idleTimer.setInterval(3000);
    connect(&m_idleTimer, &QTimer::timeout, [=] {
        if (m_applicationManager != nullptr) {
            postEvent(m_applicationManager, new QEvent(QEvent::User));
        }
    });
    m_idleTimer.start();
}

Application::~Application()
{
    //
    // Остановим все текущие соединения
    //
    NetworkRequest::stopAllConnections();
}

void Application::updateTranslation()
{
    //
    // Определим язык перевода
    //
    const int language =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/language",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    QString translationSuffix = QLocale::system().name();
    translationSuffix.truncate(translationSuffix.lastIndexOf('_'));
    QString qtTranslationSuffix = translationSuffix;
    QString qtBaseTranslationSuffix = translationSuffix;
    //
    // ... если не удалось определить локаль, используем англоязычный перевод
    //
    QLocale::Language currentLanguage = QLocale::system().language();
    if (translationSuffix.isEmpty()) {
        translationSuffix = "en";
        currentLanguage = QLocale::English;
    }

    switch (language) {
        case 0: {
            translationSuffix = "ru";
            qtTranslationSuffix = "ru";
            qtBaseTranslationSuffix = "ru";
            currentLanguage = QLocale::Russian;
            break;
        }

        case 1: {
            translationSuffix = "es";
            qtTranslationSuffix = "es";
            qtBaseTranslationSuffix = "es";
            currentLanguage = QLocale::Spanish;
            break;
        }

        case 2: {
            translationSuffix = "en";
            qtTranslationSuffix = "en";
            qtBaseTranslationSuffix = "en";
            currentLanguage = QLocale::English;
            break;
        }

        case 3: {
            translationSuffix = "fr";
            qtTranslationSuffix = "fr";
            qtBaseTranslationSuffix = "fr";
            currentLanguage = QLocale::French;
            break;
        }

        case 4: {
            translationSuffix = "kz";
            qtTranslationSuffix = "kz";
            qtBaseTranslationSuffix = "ru";
            currentLanguage = QLocale::Kazakh;
            break;
        }

        case 5: {
            translationSuffix = "ua";
            qtTranslationSuffix = "ru";
            qtBaseTranslationSuffix = "ru";
            currentLanguage = QLocale::Ukrainian;
            break;
        }

        case 6: {
            translationSuffix = "de";
            qtTranslationSuffix = "de";
            qtBaseTranslationSuffix = "de";
            currentLanguage = QLocale::German;
            break;
        }

        case 7: {
            translationSuffix = "pt";
            qtTranslationSuffix = "pt";
            qtBaseTranslationSuffix = "pt";
            currentLanguage = QLocale::Portuguese;
            break;
        }

        case 8: {
            translationSuffix = "fa";
            qtTranslationSuffix = "fa";
            currentLanguage = QLocale::Persian;
            break;
        }

        case 9: {
            translationSuffix = "zn";
            qtTranslationSuffix = "zn";
            currentLanguage = QLocale::Chinese;
            break;
        }

        case 10: {
            translationSuffix = "he";
            qtTranslationSuffix = "he";
            qtBaseTranslationSuffix = "he";
            currentLanguage = QLocale::Hebrew;
            break;
        }

        case 11: {
            translationSuffix = "pl";
            qtTranslationSuffix = "pl";
            qtBaseTranslationSuffix = "pl";
            currentLanguage = QLocale::Polish;
            break;
        }

        case 12: {
            translationSuffix = "tr";
            qtTranslationSuffix = "tr";
            qtBaseTranslationSuffix = "tr";
            currentLanguage = QLocale::Turkish;
            break;
        }

        case 13: {
            translationSuffix = "hu";
            qtTranslationSuffix = "hu";
            qtBaseTranslationSuffix = "hu";
            currentLanguage = QLocale::Turkish;
            break;
        }

        case 14: {
            translationSuffix = "it";
            qtTranslationSuffix = "it";
            qtBaseTranslationSuffix = "it";
            currentLanguage = QLocale::Italian;
            break;
        }

        case 15: {
            translationSuffix = "az";
            qtTranslationSuffix = "en";
            qtBaseTranslationSuffix = "en";
            currentLanguage = QLocale::Azerbaijani;
            break;
        }

        case 16: {
            translationSuffix = "te";
            qtTranslationSuffix = "en";
            qtBaseTranslationSuffix = "en";
            currentLanguage = QLocale::Telugu;
            break;
        }

        case 17: {
            translationSuffix = "pt_br";
            qtTranslationSuffix = "pt";
            qtBaseTranslationSuffix = "pt";
            currentLanguage = QLocale::Portuguese;
            break;
        }

        case 18: {
            translationSuffix = "sl";
            qtTranslationSuffix = "sl";
            qtBaseTranslationSuffix = "en";
            currentLanguage = QLocale::Slovenian;
            break;
        }

        case 19: {
            translationSuffix = "sv";
            qtTranslationSuffix = "sv";
            qtBaseTranslationSuffix = "en";
            currentLanguage = QLocale::Swedish;
            break;
        }

        default: break;
    }

    QLocale::setDefault(QLocale(currentLanguage));

    //
    // Подключим файл переводов Qt
    //
    static QTranslator* qtTranslator = new QTranslator;
    removeTranslator(qtTranslator);
    qtTranslator->load(":/Translations/Translations/qt_" + qtTranslationSuffix + ".qm");
    installTranslator(qtTranslator);

    //
    // Подключим дополнительный файл переводов Qt
    //
    static QTranslator* qtBaseTranslator = new QTranslator;
    removeTranslator(qtBaseTranslator);
    qtBaseTranslator->load(":/Translations/Translations/qtbase_" + qtBaseTranslationSuffix + ".qm");
    installTranslator(qtBaseTranslator);

    //
    // Подключим файл переводов программы
    //
    static QTranslator* appTranslator = new QTranslator;
    removeTranslator(appTranslator);
    appTranslator->load(":/Translations/Translations/Scenarist_" + translationSuffix + ".qm");
    installTranslator(appTranslator);

    //
    // Для языков, которые пишутся справа-налево настроим соответствующее выравнивание интерфейса
    //
    if (currentLanguage == QLocale::Persian
        || currentLanguage == QLocale::Hebrew) {
        setLayoutDirection(Qt::RightToLeft);
    } else {
        setLayoutDirection(Qt::LeftToRight);
    }
}

void Application::startApp()
{
    //
    // Получим имя файла, который пользователь возможно хочет открыть
    //
    if (m_fileToOpen.isEmpty()) {
        m_fileToOpen = arguments().value(1, QString::null);
    }
    m_applicationManager = new ManagementLayer::ApplicationManager(this);
    m_applicationManager->exec(m_fileToOpen);
    emit started();

    //
    // Запускаем остальную работу приложения
    //
    QTimer::singleShot(0, m_applicationManager, &ManagementLayer::ApplicationManager::makeStartUpChecks);
}

bool Application::notify(QObject* _object, QEvent* _event)
{
    //
    // Работа с таймером определяющим простой приложения
    //
    if (_event != nullptr
        && (_event->type() == QEvent::MouseMove
            || _event->type() == QEvent::MouseButtonPress
            || _event->type() == QEvent::MouseButtonDblClick
            || _event->type() == QEvent::KeyPress
            || _event->type() == QEvent::InputMethod
            || _event->type() == QEvent::Wheel
            || _event->type() == QEvent::Gesture)) {
        m_idleTimer.start();
    }

    return QApplication::notify(_object, _event);
}

bool Application::shouldUseHidpiScaling() const
{
    const bool hidpiScalingEnabled =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/hidpi-scaling",
                DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();
    return hidpiScalingEnabled;
}

bool Application::event(QEvent* _event)
{
    bool result = true;
    if (_event->type() == QEvent::FileOpen) {
        QFileOpenEvent* fileOpenEvent = static_cast<QFileOpenEvent*>(_event);
        if (m_applicationManager != nullptr) {
            m_applicationManager->openFile(::preparePath(fileOpenEvent->file()));
        } else {
            m_fileToOpen = ::preparePath(fileOpenEvent->file());
        }
    } else {
        result = QApplication::event(_event);
    }

    return result;
}

