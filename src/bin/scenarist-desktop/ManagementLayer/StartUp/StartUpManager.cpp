/*
* Copyright (C) 2014 Dimka Novikov, to@dimkanovikov.pro
* Copyright (C) 2016 Alexey Polushkin, armijo38@yandex.ru
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* Full license: http://dimkanovikov.pro/license/GPLv3
*/

#include "StartUpManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <UserInterfaceLayer/StartUp/StartUpView.h>
#include <UserInterfaceLayer/Application/CrashReportDialog.h>
#include <UserInterfaceLayer/Application/UpdateDialog.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/PasswordStorage.h>
#include <3rd_party/Helpers/TextEditHelper.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <NetworkRequest.h>

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QMenu>
#include <QMutableMapIterator>
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStandardPaths>
#include <QTimer>
#include <QXmlStreamReader>

using ManagementLayer::StartUpManager;
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;
using UserInterface::StartUpView;
using UserInterface::CrashReportDialog;
using UserInterface::UpdateDialog;

namespace {
    QUrl UPDATE_URL = QString("https://kitscenarist.ru/api/app/updates/");
}


StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new StartUpView(_parentWidget))
{
    initConnections();
}

QWidget* StartUpManager::view() const
{
    return m_view;
}

void StartUpManager::checkCrashReports()
{
    const QString SENDED = "sended";
    const QString IGNORED = "ignored";

    //
    // Настроим отлавливание ошибок
    //
    QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString crashReportsFolderPath = appDataFolderPath + QDir::separator() + "CrashReports";
    bool hasUnhandledReports = false;
    QString unhandledReportPath;
    for (const QFileInfo& crashReportFileInfo : QDir(crashReportsFolderPath).entryInfoList(QDir::Files)) {
        if (crashReportFileInfo.suffix() != SENDED
            && crashReportFileInfo.suffix() != IGNORED) {
            hasUnhandledReports = true;
            unhandledReportPath = crashReportFileInfo.filePath();
            break;
        }
    }

    //
    // Если есть необработанные отчёты, показать диалог
    //
    if (hasUnhandledReports) {
        CrashReportDialog dialog(m_view);

        //
        // Возьмем email из хранилища (тот же, что и для авторизации)
        //
        const QString email =
                StorageFacade::settingsStorage()->value(
                    "application/email",
                    SettingsStorage::ApplicationSettings);
        if (!email.isEmpty()) {
            dialog.setEmail(email);
        }

        QString handledReportPath = unhandledReportPath;
        if (dialog.exec() == CrashReportDialog::Accepted) {
            dialog.showProgress();
            //
            // Отправляем
            //
            NetworkRequest loader;
            loader.setRequestMethod(NetworkRequest::Post);
            loader.addRequestAttribute("version", QApplication::applicationVersion());
            loader.addRequestAttribute("email", dialog.email());
            loader.addRequestAttribute("message", dialog.message());
            loader.addRequestAttributeFile("report", unhandledReportPath);
            loader.loadSync("https://kitscenarist.ru/api/app/feedback/");

            //
            // Помечаем отчёт, как отправленный
            //
            handledReportPath += "." + SENDED;

            //
            // Сохраняем email, если ранее не было никакого
            //
            if (email.isEmpty()) {
                StorageFacade::settingsStorage()->setValue(
                            "application/email",
                            dialog.email(),
                            SettingsStorage::ApplicationSettings);
            }
        }
        //
        // Помечаем отчёт, как проигнорированный
        //
        else {
            handledReportPath += "." + IGNORED;
        }
        QFile::rename(unhandledReportPath, handledReportPath);
    }
}

void StartUpManager::checkNewVersion()
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Get);

    //
    // Сформируем uuid для приложения, по которому будем идентифицировать данного пользователя
    //
    QString uuid
            = DataStorageLayer::StorageFacade::settingsStorage()->value(
                  "application/uuid", DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                "application/uuid", uuid, DataStorageLayer::SettingsStorage::ApplicationSettings);

    //
    // Построим ссылку, чтобы учитывать запрос на проверку обновлений
    //

    loader.addRequestAttribute("system_type",
#ifdef Q_OS_WIN
                "windows"
#elif defined Q_OS_LINUX
                "linux"
#elif defined Q_OS_MAC
                "mac"
#else
                QSysInfo::kernelType()
#endif
                );

    loader.addRequestAttribute("system_name", QSysInfo::prettyProductName().toUtf8().toPercentEncoding());
    loader.addRequestAttribute("uuid", uuid);
    loader.addRequestAttribute("application_version", QApplication::applicationVersion());

    QByteArray response = loader.loadSync(UPDATE_URL);

    if (!response.isEmpty()) {
        QXmlStreamReader responseReader(response);

        const int currentLang =
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "application/language",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt();
        QString needLang;
        if (currentLang == 0
                || (currentLang == -1
                    && QLocale().language() == QLocale::Russian)) {
            needLang = "ru";
        } else {
            needLang = "en";
        }

        //
        // Распарсим ответ. Нам нужна версия, ее описание, шаблон на скачивание и является ли бетой
        //
        while (!responseReader.atEnd()) {
            responseReader.readNext();
            if (responseReader.name().toString() == "update"
                    && responseReader.tokenType() == QXmlStreamReader::StartElement) {
                QXmlStreamAttributes attrs = responseReader.attributes();
                m_updateVersion = attrs.value("version").toString();
                m_updateFileTemplate = attrs.value("file_template").toString();
                m_updateIsBeta = attrs.value("is_beta").toString() == "true"; // :)
                responseReader.readNext();
            } else if (responseReader.name().toString() == "description"
                    && responseReader.tokenType() == QXmlStreamReader::StartElement) {
                QString lang = responseReader.attributes().value("language").toString();
                if (lang == needLang) {
                    //
                    // Либо русская локаль и русский текст, либо нерусская локаль и нерусский текст
                    //
                    responseReader.readNext();
                    responseReader.readNext();
                    m_updateDescription = TextEditHelper::fromHtmlEscaped(responseReader.text().toString());
                }
                responseReader.readNext();
            }
        }

        //
        // Загрузим версию, либо которая установлена, либо которую пропустили
        //
        QString prevVersion =
                    DataStorageLayer::StorageFacade::settingsStorage()->value(
                        "application/latest_version",
                        DataStorageLayer::SettingsStorage::ApplicationSettings);

        if (m_updateVersion != QApplication::applicationVersion()
            && m_updateVersion != prevVersion
            && !QLightBoxWidget::hasOpenedWidgets()) {
            //
            // Есть новая версия, которая не совпадает с нашей. Покажем диалог
            //
            showUpdateDialog();
        }


        if (QApplication::applicationVersion() != m_updateVersion) {
            //
            // Если оказались здесь, значит либо отменили установку, либо пропустили обновление.
            // Будем показывать пользователю кнопку-ссылку на обновление
            //
            QString updateInfo = tr("Released version %1. "
                                    "<a href=\"#\" style=\"color:#2b78da;\">Install</a>").arg(m_updateVersion);
            m_view->setUpdateInfo(updateInfo);
        }
    }
}

bool StartUpManager::isOnLocalProjectsTab() const
{
    return m_view->isOnLocalProjectsTab();
}

void StartUpManager::setRecentProjects(QAbstractItemModel* _model)
{
    m_view->setRecentProjects(_model);
}

void StartUpManager::setRecentProjectName(int _index, const QString& _name)
{
    m_view->setRecentProjectName(_index, _name);
}

void StartUpManager::setRemoteProjectsVisible(bool _visible)
{
    m_view->setRemoteProjectsVisible(_visible);
}

void StartUpManager::setRemoteProjects(QAbstractItemModel* _model)
{
    m_view->setRemoteProjects(_model);
}

void StartUpManager::setRemoteProjectName(int _index, const QString& _name)
{
    m_view->setRemoteProjectName(_index, _name);
}

#ifdef Q_OS_MAC
void StartUpManager::buildEditMenu(QMenu* _menu)
{
    auto* undo = _menu->addAction(tr("Undo"));
    undo->setShortcut(QKeySequence::Undo);
    undo->setEnabled(false);
    auto* redo = _menu->addAction(tr("Redo"));
    redo->setShortcut(QKeySequence::Redo);
    redo->setEnabled(false);
}
#endif

void StartUpManager::downloadUpdate(const QString &_fileTemplate)
{
    NetworkRequest loader;

    connect(&loader, &NetworkRequest::downloadProgress, this, &StartUpManager::downloadProgressForUpdate);
    connect(this, &StartUpManager::stopDownloadForUpdate, &loader, &NetworkRequest::stop);

    loader.setRequestMethod(NetworkRequest::Get);
    loader.clearRequestAttributes();

    //
    // Языковой суффикс
    //
    QString localeSuffix;
    if (QLocale().language() == QLocale::English) {
        localeSuffix = "_en";
    } else if (QLocale().language() == QLocale::Spanish) {
        localeSuffix = "_es";
    } else if (QLocale().language() == QLocale::French) {
        localeSuffix = "_fr";
    }

    //
    // URL до новой версии в соответствии с ОС, архитектурой и языком
    //
#ifdef Q_OS_WIN
    QString updateUrl = QString("windows/%1.exe").arg(_fileTemplate);
#elif defined Q_OS_LINUX
    #ifdef Q_PROCESSOR_X86_32
        QString arch = "_i386";
    #else
        QString arch = "_amd64";
    #endif

    QString updateUrl = QString("linux/%1%2%3.deb").arg(_fileTemplate, localeSuffix, arch);
#elif defined Q_OS_MAC
    QString updateUrl = QString("mac/%1%2.dmg").arg(_fileTemplate, localeSuffix);
#endif

    //
    // Загружаем установщик
    //
    const QString prefixUrl = "https://kitscenarist.ru/downloads/";
    QUrl updateInfoUrl(prefixUrl + updateUrl);
    QByteArray response = loader.loadSync(updateInfoUrl);
    if (response.isEmpty()) {
        emit errorDownloadForUpdate();
        return;
    }

    //
    // Сохраняем установщик в файл
    //
    const QString tempDirPath = QDir::toNativeSeparators(QDir::tempPath());
    m_updateFile = tempDirPath + QDir::separator() + updateInfoUrl.fileName();
    QFile tempFile(m_updateFile);
    if (tempFile.open(QIODevice::WriteOnly)) {
        tempFile.write(response);
        tempFile.close();
        emit downloadFinishedForUpdate();
    }
}

void StartUpManager::showUpdateDialog()
{
    UpdateDialog dialog(m_view);

    bool isSupported = true;

    connect(&dialog, &UpdateDialog::skipUpdate, [this] {
        StorageFacade::settingsStorage()->setValue(
                    "application/latest_version",
                    m_updateVersion,
                    SettingsStorage::ApplicationSettings);
    });
    connect(&dialog, &UpdateDialog::downloadUpdate, [this] {
        downloadUpdate(m_updateFileTemplate);
    });

    connect(this, &StartUpManager::downloadProgressForUpdate, &dialog, &UpdateDialog::setProgressValue);
    connect(this, &StartUpManager::downloadFinishedForUpdate, &dialog, &UpdateDialog::downloadFinished);
    connect(this, &StartUpManager::errorDownloadForUpdate, &dialog, &UpdateDialog::showDownloadError);

#ifdef Q_OS_LINUX
    isSupported = false;
    const QString distroName = QSysInfo::prettyProductName().toLower();
    const QStringList supportedDistros({"ubuntu", "mint"});
    for (const QString &supportedDistro : supportedDistros) {
        if (distroName.contains(supportedDistro)) {
            isSupported = true;
            break;
        }
    }
#endif

    //
    // Для версий основанных на старых сборках отключаем автоматическую установку обновлений
    //
    if (QApplication::applicationVersion().contains("granny")) {
        isSupported = false;
    }

    //
    // Покажем окно с обновлением
    //
    bool needToShowUpdate = true;
    QString updateDialogText = m_updateDescription;
    do {
        const int showResult =
                dialog.showUpdate(m_updateVersion, updateDialogText, m_updateIsBeta, isSupported);
        if (showResult == UpdateDialog::Accepted) {
            //
            // Нажали "Установить"
            //
#ifdef Q_OS_WIN
            if (QProcess::startDetached(m_updateFile)) {
#elif defined Q_OS_LINUX
            if (QProcess::startDetached("software-center", { m_updateFile })) {
#else
            if (QDesktopServices::openUrl(QUrl::fromLocalFile(m_updateFile))) {
#endif
                exit(0);
            } else {
                updateDialogText = tr("Can't install update. There are some problems with downloaded file.\n\nYou can try to reload update.");
            }
        } else {
            needToShowUpdate = false;
        }
    } while (needToShowUpdate);

    //
    // Отменили или пропустили. Остановим загрузку
    //
    emit stopDownloadForUpdate();
}

void StartUpManager::initConnections()
{
    connect(m_view, &StartUpView::createProjectClicked, this, &StartUpManager::createProjectRequested);
    connect(m_view, &StartUpView::openProjectClicked, this, &StartUpManager::openProjectRequested);
    connect(m_view, &StartUpView::helpClicked, this, &StartUpManager::helpRequested);
    connect(m_view, &StartUpView::updateRequested, this, &StartUpManager::showUpdateDialog);

    connect(m_view, &StartUpView::openRecentProjectClicked, this, &StartUpManager::openRecentProjectRequested);
    connect(m_view, &StartUpView::hideRecentProjectRequested, this, &StartUpManager::hideRecentProjectRequested);
    connect(m_view, &StartUpView::openRemoteProjectClicked, this, &StartUpManager::openRemoteProjectRequested);
    connect(m_view, &StartUpView::editRemoteProjectRequested, this, &StartUpManager::editRemoteProjectRequested);
    connect(m_view, &StartUpView::removeRemoteProjectRequested, this, &StartUpManager::removeRemoteProjectRequested);
    connect(m_view, &StartUpView::shareRemoteProjectRequested, this, &StartUpManager::shareRemoteProjectRequested);
    connect(m_view, &StartUpView::unshareRemoteProjectRequested, this, &StartUpManager::unshareRemoteProjectRequested);
    connect(m_view, &StartUpView::refreshProjects, this, &StartUpManager::refreshProjectsRequested);
}
