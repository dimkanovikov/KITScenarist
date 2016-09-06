#include "SynchronizationManagerV2.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <QEventLoop>
#include <QTimer>

using ManagementLayer::SynchronizationManagerV2;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;

SynchronizationManagerV2::SynchronizationManagerV2(QObject* _parent, QWidget* _parentView) :
    QObject(_parent),
    m_view(_parentView)
{

}

void SynchronizationManagerV2::autoLogin()
{
    //
    // Получим параметры из хранилища
    //
    const QString email =
            StorageFacade::settingsStorage()->value(
                "application/email",
                SettingsStorage::ApplicationSettings);
    const QString password =
            PasswordStorage::load(
                StorageFacade::settingsStorage()->value(
                    "application/password",
                    SettingsStorage::ApplicationSettings),
                email
                );

    //
    // Если они не пусты, авторизуемся
    //
    if (!email.isEmpty() && !password.isEmpty()) {
        login(email, password);
    }
}

void SynchronizationManagerV2::login(const QString &_email, const QString &_password)
{
    bool success = false;

    QEventLoop event;
    QTimer::singleShot(2000, &event, SLOT(quit()));
    event.exec();
    //
    // Авторизация
    //
    if((_email == "admin" && _password == "admin")
            || (_email == "user" && _password == "user")) { // :)
        success = true;
        m_sessionKey = "12345";
    }
    else {
        handleError(tr("Wrong email or password"), 100);
    }

    if(success) {
        //
        // Если авторизация успешна, сохраним информацию о пользователе
        //
        StorageFacade::settingsStorage()->setValue(
                    "application/email",
                    _email,
                    SettingsStorage::ApplicationSettings);
        StorageFacade::settingsStorage()->setValue(
                    "application/password",
                    PasswordStorage::save(_password, _email),
                    SettingsStorage::ApplicationSettings);

        emit loginAccepted();
    }

}

void SynchronizationManagerV2::registration(const QString& _email, const QString& _password,
                                            const QString& _type)
{
    QEventLoop event;
    QTimer::singleShot(2000, &event, SLOT(quit()));
    event.exec();

    bool success = false;
    if(_email == "user" && _password == "user") {
        success = true;
    }
    else {
        handleError("Wrong email", 404);
    }

    if(success) {
        emit registered();
    }
}

void SynchronizationManagerV2::verifyRegistration(const QString& _code)
{
    QEventLoop event;
    QTimer::singleShot(2000, &event, SLOT(quit()));
    event.exec();

    bool success = false;
    if(_code == "11111") {
        success = true;
    }
    else {
        handleError("Wrong code", 505);
    }

    if(success) {
        emit registerVerified();
    }
}

void SynchronizationManagerV2::recoveryPassword(const QString &_email)
{
    QEventLoop event;
    QTimer::singleShot(2000, &event, SLOT(quit()));
    event.exec();

    bool success = false;
    if(_email == "recovery") {
        success = true;
    }
    else {
        handleError("Wrong email", 606);
    }

    if(success) {
        emit passwordRecoveried();
    }
}

void SynchronizationManagerV2::handleError(const QString &_error, int _code)
{
    m_sessionKey.clear();
    emit syncClosedWithError(_code, _error);
}
