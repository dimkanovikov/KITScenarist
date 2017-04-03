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
#include <UserInterfaceLayer/StartUp/LoginDialog.h>
#include <UserInterfaceLayer/StartUp/ChangePasswordDialog.h>
#include <UserInterfaceLayer/StartUp/RenewSubscriptionDialog.h>
#include <UserInterfaceLayer/StartUp/CrashReportDialog.h>
#include <UserInterfaceLayer/StartUp/UpdateDialog.h>

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
#include <QMutableMapIterator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
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
using UserInterface::LoginDialog;
using UserInterface::ChangePasswordDialog;
using UserInterface::RenewSubscriptionDialog;
using UserInterface::CrashReportDialog;
using UserInterface::UpdateDialog;

namespace {
    QUrl UPDATE_URL = QString("https://kitscenarist.ru/api/app/updates/");
}


StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new StartUpView(_parentWidget)),
    m_loginDialog(new LoginDialog(m_view)),
    m_changePasswordDialog(new ChangePasswordDialog(m_view)),
    m_renewSubscriptionDialog(new RenewSubscriptionDialog(m_view))
{
    initView();

    initData();
    initConnections();

    QTimer::singleShot(0, [=] {
        //
        // Проверим наличие отчётов об ошибках в работе программы
        //
        checkCrashReports();

        //
        // Проверяем наличие новой версии уже после старта программы
        //
        checkNewVersion();

        emit initialized();
    });
}

QWidget* StartUpManager::view() const
{
    return m_view;
}

void StartUpManager::setProgressLoginLabel(bool _enable)
{
    if (_enable) {
        m_view->enableProgressLoginLabel(0, true);
    } else {
        m_view->disableProgressLoginLabel();
    }
}

bool StartUpManager::isOnLoginDialog() const
{
    return m_loginDialog->isVisible() || m_changePasswordDialog->isVisible();
}

bool StartUpManager::isOnLocalProjectsTab() const
{
    return m_view->isOnLocalProjectsTab();
}

void StartUpManager::completeLogin(const QString& _userName, const QString& _userEmail,
                                   int _paymentMonth)
{
    m_view->disableProgressLoginLabel();
    m_userEmail = _userEmail;

    m_renewSubscriptionDialog->setPaymentMonth(_paymentMonth);

    const bool isLogged = true;
    m_view->setUserLogged(isLogged, _userName, m_userEmail);
    m_loginDialog->unblock();
    m_loginDialog->hide();
}

void StartUpManager::verifyUser()
{
    //
    // Покажем пользователю окно с вводом проверочного кода
    //
    m_loginDialog->showVerificationSuccess();
}

void StartUpManager::userAfterSignUp()
{
    //
    // После того, как пользователь зарегистрировался, сразу выполним вход
    //
    emit loginRequested(m_loginDialog->signUpEmail(), m_loginDialog->signUpPassword());
}

void StartUpManager::userPassRestored()
{
    m_loginDialog->showRestoreSuccess();
}

void StartUpManager::completeLogout()
{
    m_userEmail.clear();

    const bool isLogged = false;
    m_view->setUserLogged(isLogged);
}

void StartUpManager::passwordChanged()
{
    m_changePasswordDialog->stopAndHide();
    QLightBoxMessage::information(m_view, QString::null, tr("Password successfully changed"));
}

void StartUpManager::showPasswordError(const QString& _error)
{
    if (m_loginDialog->isVisible()) {
        //
        // Если активно окно авторизации, то покажем ошибку там
        //
        retrySignUp(_error);
    } else {
        //
        // Иначе, активно окно смены пароля
        //
        m_changePasswordDialog->stopAndHide();
        QLightBoxMessage::critical(m_view, tr("Can not change password"),
                                   _error);
        m_changePasswordDialog->showUnprepared();
    }
}

void StartUpManager::setSubscriptionInfo(bool _isActive, const QString &_expiredDate)
{
    if (m_renewSubscriptionDialog->isVisible()) {
        //
        // Если окно продления подписки показано, значит,
        // необходимо обновлять, пока не получим изменения
        //
        if (_expiredDate != m_subscriptionEndDate) {
            //
            // Обновилось, обновим окно и поле в StartUpView
            //
            m_renewSubscriptionDialog->showThanks(_expiredDate);
            m_view->setSubscriptionInfo(_isActive, _expiredDate);
        } else {
            //
            // Не обновилось, запросим еще раз
            //
            QTimer::singleShot(3000, this, &StartUpManager::getSubscriptionInfoRequested);
        }
    } else {
        //
        // Иначе, это обычный запрос на обновление
        //
        m_subscriptionEndDate = _expiredDate;
        m_view->setSubscriptionInfo(_isActive, _expiredDate);
    }
}

void StartUpManager::setRecentProjects(QAbstractItemModel* _model)
{
    m_view->setRecentProjects(_model);
}

void StartUpManager::setRemoteProjects(QAbstractItemModel* _model)
{
    m_view->setRemoteProjects(_model);
}

void StartUpManager::retryLogin(const QString& _error)
{
    //
    // Покажем пользователю ошибку авторизации
    //
    m_loginDialog->setLoginError(_error);
}

void StartUpManager::retrySignUp(const QString &_error)
{
    //
    // Покажем пользователю ошибку регистрации
    //
    m_loginDialog->setSignUpError(_error);
}

void StartUpManager::retryVerify(const QString &_error)
{
    //
    // Покажем пользователю ошибку ввода проверочного кода
    //
    m_loginDialog->setVerificationError(_error);
}

void StartUpManager::retryLastAction(const QString &_error)
{
    if (m_loginDialog->isVisible()) {
        m_loginDialog->setLastActionError(_error);
    } else if(m_changePasswordDialog->isVisible()) {
        m_changePasswordDialog->stopAndHide();
        QLightBoxMessage::critical(m_view, tr("Can not change password"),
                                   _error);
        m_changePasswordDialog->showUnprepared();
    }
}

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
    QString distroName = QSysInfo::prettyProductName().toLower();
    QStringList supportedDistros({"ubuntu", "mint", "elementary", "debian"});
    for(const QString &supportedDistro : supportedDistros) {
        if (distroName.contains(supportedDistro)) {
            isSupported = true;
            break;
        }
    }
#endif

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

void StartUpManager::initData()
{
}

void StartUpManager::initView()
{
}

void StartUpManager::initConnections()
{
    //
    // Показать пользователю диалог авторизации/регистрации
    // Предварительно его очистив
    //
    connect(m_view, &StartUpView::loginClicked, [this] {
        m_loginDialog->showPrepared();
    });

    connect(m_view, &StartUpView::logoutClicked, this, &StartUpManager::logoutRequested);
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

    connect(m_loginDialog, &LoginDialog::loginRequested, [this] {
        emit loginRequested(m_loginDialog->loginEmail(),
                            m_loginDialog->loginPassword());}
    );
    connect(m_loginDialog, &LoginDialog::signUpRequested, [this] {
        emit signUpRequested(m_loginDialog->signUpEmail(),
                             m_loginDialog->signUpPassword());
    });
    connect(m_loginDialog, &LoginDialog::verifyRequested, [this] {
        emit verifyRequested(m_loginDialog->verificationCode());
    });
    connect(m_loginDialog, &LoginDialog::restoreRequested, [this] {
        emit restoreRequested(m_loginDialog->loginEmail());
    });
    connect(m_view, &StartUpView::userNameChanged,
            this, &StartUpManager::userNameChangeRequested);
    connect(m_view, &StartUpView::getSubscriptionInfoClicked,
            this, &StartUpManager::getSubscriptionInfoRequested);
    connect(m_view, &StartUpView::renewSubscriptionClicked,
            m_renewSubscriptionDialog, &RenewSubscriptionDialog::showPrepared);
    connect(m_view, &StartUpView::passwordChangeClicked, [this] {
        m_changePasswordDialog->showPrepared();
    });

    connect(m_changePasswordDialog, &ChangePasswordDialog::changeRequested, [this] {
        emit passwordChangeRequested(m_changePasswordDialog->password(),
                                     m_changePasswordDialog->newPassword());
    });

    connect(m_renewSubscriptionDialog, &RenewSubscriptionDialog::renewSubsciptionRequested, [this] {
        emit renewSubscriptionRequested(m_renewSubscriptionDialog->duration(), m_renewSubscriptionDialog->paymentSystemType());
    });
    connect(m_renewSubscriptionDialog, &RenewSubscriptionDialog::renewSubsciptionRequested, [this] {
        QTimer::singleShot(3000, this, &StartUpManager::getSubscriptionInfoRequested);
    });
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
