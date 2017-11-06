#include "OnboardingManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <UserInterfaceLayer/Onboarding/OnboardingView.h>

#include <QApplication>
#include <QFile>
#include <QStyle>
#include <QStyleFactory>

using ManagementLayer::OnboardingManager;
using UserInterface::OnboardingView;


OnboardingManager::OnboardingManager(QObject* _parent) :
    QObject(_parent),
    m_view(new OnboardingView)
{
    initConnections();
}

OnboardingManager::~OnboardingManager()
{
    m_view->deleteLater();
}

bool OnboardingManager::needConfigureApp() const
{
    return true;
    const bool isAppConfigured =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/app-was-configured",
                DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();
    return !isAppConfigured;
}

void OnboardingManager::exec()
{
    updateUsedTemplates();
    setUseDarkTheme(true);

    m_view->show();
}

void OnboardingManager::close()
{
    m_view->close();
}

void OnboardingManager::initConnections()
{
    connect(m_view, &OnboardingView::languageChanged, this, &OnboardingManager::setLanguage);
    connect(m_view, &OnboardingView::useDarkThemeToggled, this, &OnboardingManager::setUseDarkTheme);
    connect(m_view, &OnboardingView::skipClicked, this, &OnboardingManager::skip);
    connect(m_view, &OnboardingView::finishClicked, this, &OnboardingManager::finalize);
}

void OnboardingManager::updateUsedTemplates()
{
    auto newTemplateName = [] (const QString& _oldName) {
        if (_oldName == BusinessLogic::ScenarioTemplate::oldFinalDraftA4TemplateName()) {
            return BusinessLogic::ScenarioTemplate::finalDraftA4TemplateName();
        } else if (_oldName == BusinessLogic::ScenarioTemplate::oldFinalDraftLetterTemplateName()) {
            return BusinessLogic::ScenarioTemplate::finalDraftLetterTemplateName();
        } else if (_oldName == BusinessLogic::ScenarioTemplate::oldRussianTemplateName()) {
            return BusinessLogic::ScenarioTemplate::russianTemplateName();
        } else if (_oldName == BusinessLogic::ScenarioTemplate::oldRussianWithCourierPrimeTemplateName()) {
            return BusinessLogic::ScenarioTemplate::russianTemplateWithCourierPrimeTemplateName();
        } else if (_oldName == BusinessLogic::ScenarioTemplate::oldChineseTemplateName()) {
            return BusinessLogic::ScenarioTemplate::chineseTemplateName();
        } else if (_oldName == BusinessLogic::ScenarioTemplate::oldHebrewTemplateName()) {
            return BusinessLogic::ScenarioTemplate::hebrewTemplateName();
        }

        return _oldName;
    };

    //
    // Заменяем имена старых стилей на новые
    //
    const QString editorTemplate =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "scenario-editor/current-style",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "scenario-editor/current-style",
                newTemplateName(editorTemplate),
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    //
    const QString exportTemplate =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "export/style",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "export/style",
                newTemplateName(exportTemplate),
                DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void OnboardingManager::setLanguage(int _language)
{
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/language",
                QString::number(_language),
                DataStorageLayer::SettingsStorage::ApplicationSettings);

    emit translationUpdateRequested();
}

void OnboardingManager::setUseDarkTheme(bool _useDarkTheme)
{
    //
    // Настраиваем палитру и стилевые надстройки в зависимости от темы
    //
    QPalette palette = QStyleFactory::create("Fusion")->standardPalette();

    if (_useDarkTheme) {
        palette.setColor(QPalette::Window, QColor("#26282a"));
        palette.setColor(QPalette::WindowText, QColor("#ebebeb"));
        palette.setColor(QPalette::Button, QColor("#414244"));
        palette.setColor(QPalette::ButtonText, QColor("#ebebeb"));
        palette.setColor(QPalette::Base, QColor("#404040"));
        palette.setColor(QPalette::AlternateBase, QColor("#353535"));
        palette.setColor(QPalette::Text, QColor("#ebebeb"));
        palette.setColor(QPalette::Highlight, QColor("#2b78da"));
        palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
        palette.setColor(QPalette::Light, QColor("#404040"));
        palette.setColor(QPalette::Midlight, QColor("#424140"));
        palette.setColor(QPalette::Dark, QColor("#696765"));
        palette.setColor(QPalette::Shadow, QColor("#1c2023"));

        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#a1a1a1"));
        palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#1b1e21"));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#a1a1a1"));
        palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#333333"));
        palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#bcbdbf"));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#666769"));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
    } else {
        palette.setColor(QPalette::Window, QColor("#f6f6f6"));
        palette.setColor(QPalette::WindowText, QColor("#38393a"));
        palette.setColor(QPalette::Button, QColor("#e4e4e4"));
        palette.setColor(QPalette::ButtonText, QColor("#38393a"));
        palette.setColor(QPalette::Base, QColor("#ffffff"));
        palette.setColor(QPalette::AlternateBase, QColor("#eeeeee"));
        palette.setColor(QPalette::Text, QColor("#38393a"));
        palette.setColor(QPalette::Highlight, QColor("#2b78da"));
        palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
        palette.setColor(QPalette::Light, QColor("#ffffff"));
        palette.setColor(QPalette::Midlight, QColor("#d6d6d6"));
        palette.setColor(QPalette::Dark, QColor("#bdbebf"));
        palette.setColor(QPalette::Mid, QColor("#a0a2a4"));
        palette.setColor(QPalette::Shadow, QColor("#585a5c"));

        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#acadaf"));
        palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#f6f6f6"));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#acadaf"));
        palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#595a5c"));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#acadaf"));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
    }

    //
    // Применяем палитру
    //
    qApp->setPalette(palette);

    //
    // Чтобы все цветовые изменения подхватились, нужно заново переустановить стиль
    //
    QFile styleSheetFile(
                QString(":/Interface/UI/style-desktop%1.qss")
                .arg(QLocale().textDirection() == Qt::RightToLeft ? "-rtl" : ""));
    styleSheetFile.open(QIODevice::ReadOnly);
    QString styleSheet = styleSheetFile.readAll();
    styleSheetFile.close();
    styleSheet.replace("_THEME_POSTFIX", _useDarkTheme ? "-dark" : "");
    m_view->setStyleSheet(styleSheet);
}

void OnboardingManager::skip()
{
    //
    // Запоминаем, что первоначальная настройка была выполнена
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/app-was-configured", "1",
                DataStorageLayer::SettingsStorage::ApplicationSettings);

    //
    // И просто перейдём к приложению, не применяя настройки
    //
    emit finished();
}

void OnboardingManager::finalize()
{
    //
    // Запоминаем, что первоначальная настройка была выполнена
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/app-was-configured", "1",
                DataStorageLayer::SettingsStorage::ApplicationSettings);

    //
    // Язык сохранился и применился в момент выбора, поэтому сохраняем все остальные парматеры
    //
    // ... цветовая схема
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/use-dark-theme",
                m_view->isUseDarkTheme() ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    //
    // ... использование модулей
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/modules/research",
                m_view->isUseResearch() ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/modules/cards",
                m_view->isUseCards() ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/modules/scenario",
                m_view->isUseScript() ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/modules/statistics",
                m_view->isUseStatistics() ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    //
    // ... шаблон сценария
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "scenario-editor/current-style",
                m_view->scriptTemplate(),
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "export/style",
                m_view->scriptTemplate(),
                DataStorageLayer::SettingsStorage::ApplicationSettings);

    emit finished();
}
