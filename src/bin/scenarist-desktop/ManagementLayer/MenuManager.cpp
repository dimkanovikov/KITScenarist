#include "MenuManager.h"

#include <UserInterfaceLayer/Account/ChangePasswordDialog.h>
#include <UserInterfaceLayer/Account/LoginDialog.h>
#include <UserInterfaceLayer/Account/RenewSubscriptionDialog.h>
#include <UserInterfaceLayer/Application/MenuView.h>
#include <UserInterfaceLayer/Application/AboutDialog.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>

#include <NetworkRequestLoader.h>

#include <QCryptographicHash>
#include <QTimer>

using ManagementLayer::MenuManager;
using UserInterface::MenuView;
using UserInterface::LoginDialog;
using UserInterface::ChangePasswordDialog;
using UserInterface::RenewSubscriptionDialog;

namespace {
    /**
     * @brief Получить стандартную аватарку
     */
    static const QPixmap defaultAvatar() {
        return QPixmap(":/Graphics/Images/avatar.png");
    }
}


MenuManager::MenuManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new MenuView(_parentWidget)),
    m_loginDialog(new LoginDialog(_parentWidget)),
    m_changePasswordDialog(new ChangePasswordDialog(_parentWidget)),
    m_renewSubscriptionDialog(new RenewSubscriptionDialog(_parentWidget))
{
    initView();
    initConnections();
}

void MenuManager::setMenu(QMenu* _menu)
{
    m_view->setMenu(_menu);
}

QMenu* MenuManager::menu() const
{
    return m_view->menu();
}

void MenuManager::setMenuItemEnabled(int _index, bool _enabled)
{
    m_view->setMenuItemEnabled(_index, _enabled);
}

void MenuManager::disableProjectActions()
{
    m_view->disableProjectActions();
}

void MenuManager::enableProjectActions()
{
    m_view->enableProjectActions();
}

void MenuManager::showMenu()
{
    const bool animate = false;
    m_view->showMenuPage(animate);
    WAF::Animation::sideSlideIn(m_view);
}

void MenuManager::setProgressLoginLabel(bool _enable)
{
    if (_enable) {
        m_view->enableProgressLoginLabel(0, true);
    } else {
        m_view->disableProgressLoginLabel();
    }
}

void MenuManager::completeLogin(const QString& _userName, const QString& _userEmail, int _paymentMonth)
{
    m_view->disableProgressLoginLabel();
    m_userEmail = _userEmail;

    m_renewSubscriptionDialog->setPaymentMonth(_paymentMonth);

    const bool isLogged = true;
    m_view->setUserLogged(isLogged, _userName, m_userEmail);
    m_loginDialog->unblock();
    m_loginDialog->hide();

    //
    // Загрузим аватарку пользователя
    //
    const QString emailHash = QCryptographicHash::hash(_userEmail.toLower().toUtf8(), QCryptographicHash::Md5).toHex();
    const QString avatarUrl = QString("https://www.gravatar.com/avatar/%1?s=45&d=404").arg(emailHash);
    NetworkRequestLoader::loadAsync(avatarUrl, [this] (const QByteArray& _avatarData) {
        QPixmap avatar;
        //
        // Если аватар не найден используем стандартную аватарку
        //
        if (_avatarData == "404 Not Found"
            || _avatarData.isEmpty()) {
            avatar = defaultAvatar();
        }
        //
        // В противном случае используем полученное изображение
        //
        else {
            avatar.loadFromData(_avatarData);
        }
        m_view->setAvatar(avatar);
    });
}

void MenuManager::setSubscriptionInfo(bool _isActive, const QString& _expiredDate, quint64 _usedSpace, quint64 _availableSpace)
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
            m_view->setSubscriptionInfo(_isActive, _expiredDate, _usedSpace, _availableSpace);
        } else {
            //
            // Не обновилось, запросим еще раз
            //
            QTimer::singleShot(3000, this, &MenuManager::getSubscriptionInfoRequested);
        }
    } else {
        //
        // Иначе, это обычный запрос на обновление
        //
        m_subscriptionEndDate = _expiredDate;
        m_view->setSubscriptionInfo(_isActive, _expiredDate, _usedSpace, _availableSpace);
    }
}

void MenuManager::verifyUser()
{
    //
    // Покажем пользователю окно с вводом проверочного кода
    //
    m_loginDialog->showVerificationSuccess();
}

void MenuManager::userAfterSignUp()
{
    //
    // После того, как пользователь зарегистрировался, сразу выполним вход
    //
    emit loginRequested(m_loginDialog->signUpEmail(), m_loginDialog->signUpPassword());
}

void MenuManager::userPassRestored()
{
    m_loginDialog->showRestoreSuccess();
}

void MenuManager::completeLogout()
{
    m_userEmail.clear();

    const bool isLogged = false;
    m_view->setUserLogged(isLogged);
    m_view->setAvatar(defaultAvatar());
}

void MenuManager::passwordChanged()
{
    m_changePasswordDialog->stopAndHide();
    QLightBoxMessage::information(m_view, QString::null, tr("Password successfully changed"));
}

void MenuManager::showPasswordError(const QString& _error)
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

void MenuManager::retryLogin(const QString& _error)
{
    //
    // Покажем пользователю ошибку авторизации
    //
    m_loginDialog->setLoginError(_error);
}

void MenuManager::retrySignUp(const QString& _error)
{
    //
    // Покажем пользователю ошибку регистрации
    //
    m_loginDialog->setSignUpError(_error);
}

void MenuManager::retryVerify(const QString& _error)
{
    //
    // Покажем пользователю ошибку ввода проверочного кода
    //
    m_loginDialog->setVerificationError(_error);
}

void MenuManager::retryLastAction(const QString& _error)
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

bool MenuManager::isOnLoginDialog() const
{
    return m_loginDialog->isVisible() || m_changePasswordDialog->isVisible();
}

void MenuManager::initView()
{
    m_view->hide();
}

void MenuManager::initConnections()
{
    connect(m_view, &MenuView::loginPressed, m_loginDialog, &LoginDialog::showPrepared);
    connect(m_view, &MenuView::userNameChanged, this, &MenuManager::userNameChangeRequested);
    connect(m_view, &MenuView::getSubscriptionInfoClicked, this, &MenuManager::getSubscriptionInfoRequested);
    connect(m_view, &MenuView::renewSubscriptionClicked, m_renewSubscriptionDialog, &RenewSubscriptionDialog::showPrepared);
    connect(m_view, &MenuView::passwordChangeClicked, m_changePasswordDialog, &ChangePasswordDialog::showPrepared);
    connect(m_view, &MenuView::logoutClicked, this, &MenuManager::logoutRequested);

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

    connect(m_changePasswordDialog, &ChangePasswordDialog::changeRequested, [this] {
        emit passwordChangeRequested(m_changePasswordDialog->password(),
                                     m_changePasswordDialog->newPassword());
    });

    connect(m_renewSubscriptionDialog, &RenewSubscriptionDialog::renewSubsciptionRequested, [this] {
        emit renewSubscriptionRequested(m_renewSubscriptionDialog->duration(), m_renewSubscriptionDialog->paymentSystemType());
        QTimer::singleShot(3000, this, &MenuManager::getSubscriptionInfoRequested);
    });

    connect(m_view, &MenuView::aboutAppPressed, [this] {
        UserInterface::AboutDialog dlg(m_view);
        dlg.exec();
    });

    connect(m_view, &MenuView::hideRequested, [this] { WAF::Animation::sideSlideOut(m_view); });
}
