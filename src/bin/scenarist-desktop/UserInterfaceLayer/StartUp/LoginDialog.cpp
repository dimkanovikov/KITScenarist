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

#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include <3rd_party/Helpers/Validators.h>

using UserInterface::LoginDialog;

namespace {
    /**
     * @brief Размер иконки
     */
    const QSize ICON_PIXMAP_SIZE(32, 32);
}


LoginDialog::LoginDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::LoginDialog),
    m_isVerify(false)
{
    m_ui->setupUi(this);
    m_loginButton = m_ui->loginButtons->addButton(tr("Login"), QDialogButtonBox::AcceptRole);
    m_signUpButton = m_ui->signUpButtons->addButton(tr("Sign Up"), QDialogButtonBox::AcceptRole);
}

LoginDialog::~LoginDialog()
{
    delete m_ui;
}

QString LoginDialog::loginEmail() const
{
    return m_ui->loginEmail->text();
}

QString LoginDialog::signUpEmail() const
{
    return m_ui->signUpEmail->text();
}

QString LoginDialog::loginPassword() const
{
    return m_ui->loginPasswordEdit->text();
}

QString LoginDialog::signUpPassword() const
{
    return m_ui->signUpPasswordEdit->text();
}

QString LoginDialog::verificationCode() const
{
    return m_ui->verificationCode->text();
}

void LoginDialog::setLoginError(const QString& _error)
{
    updateLabel(m_ui->loginError, m_ui->loginErrorIcon, _error, true);
    unblock();
}

void LoginDialog::setSignUpError(const QString &_error)
{
    updateLabel(m_ui->signUpError, m_ui->signUpErrorIcon, _error, true);
    unblock();
}

void LoginDialog::setVerificationError(const QString &_error)
{
    updateLabel(m_ui->verificationError, m_ui->verificationErrorIcon,_error, true);
    unblock();
}

void LoginDialog::setLastActionError(const QString &_error)
{
    if (m_ui->tabs->currentIndex() == 0) {
        setLoginError(_error);
    } else if (m_isVerify) {
        setVerificationError(_error);
    } else {
        setSignUpError(_error);
    }
}

void LoginDialog::showVerificationSuccess()
{
    m_isVerify = true;
    updateLabel(m_ui->verificationError, m_ui->verificationErrorIcon,
                tr("Your e-mail <b>%1</b> was sent a letter with a "
                   "confirmation code.").arg(m_ui->signUpEmail->text()), false);
    m_ui->stackedWidget->setCurrentWidget(m_ui->verificationPage);

    unblock();
    m_ui->verificationCode->setFocus();
}

void LoginDialog::showRestoreSuccess()
{
    updateLabel(m_ui->loginError, m_ui->loginErrorIcon,
                tr("Your e-mail <b>%1</b> was sent a letter with a password.").
                arg(m_ui->loginEmail->text()),
                  false);
    m_ui->restorePassword->setEnabled(false);

    unblock();
}

void LoginDialog::showPrepared()
{
    clear();
    QLightBoxDialog::show();
    m_ui->loginEmail->setFocus();
}

void LoginDialog::unblock()
{
    m_ui->tabs->setEnabled(true);

    m_ui->stackedWidget->setEnabled(true);

    hideProgress();
}

QWidget* LoginDialog::focusedOnExec() const
{
    return m_ui->loginEmail;
}

QWidget* LoginDialog::titleWidget() const
{
    return m_ui->tabs;
}

void LoginDialog::checkVerificationCode()
{
    QRegExpValidator validator(QRegExp("[0-9]{5}"));
    QString s = m_ui->verificationCode->text();
    int pos = 0;
    if (validator.validate(s, pos) == QValidator::Acceptable) {
        block();
        emit verifyRequested();
    }
}

void LoginDialog::tryLogin()
{
    if (Validator::isEmailValid(m_ui->loginEmail->text())) {
        m_ui->loginError->clear();
        m_ui->loginErrorIcon->clear();
        block();
        emit loginRequested();
    } else {
        updateLabel(m_ui->loginError, m_ui->loginErrorIcon, tr("Email is invalid"), true);
    }
}

void LoginDialog::trySignUp()
{
    if (Validator::isEmailValid(m_ui->signUpEmail->text())) {
        m_ui->signUpError->clear();
        m_ui->signUpErrorIcon->clear();
        block();
        emit signUpRequested();
    } else {
        updateLabel(m_ui->signUpError, m_ui->signUpErrorIcon, tr("Email is invalid"), true);
    }
}

void LoginDialog::cancelVerify()
{
    m_isVerify = false;
    switchWidget();
}

void LoginDialog::switchWidget()
{
    if (m_ui->tabs->currentIndex() == 0) {
        m_ui->stackedWidget->setCurrentWidget(m_ui->loginPage);
        m_ui->loginEmail->setFocus();
    } else if (m_isVerify) {
        m_ui->stackedWidget->setCurrentWidget(m_ui->verificationPage);
        m_ui->verificationCode->setFocus();
    } else {
        m_ui->stackedWidget->setCurrentWidget(m_ui->signUpPage);
        m_ui->signUpEmail->setFocus();
    }
}

void LoginDialog::block()
{
    m_ui->tabs->setEnabled(false);

    m_ui->stackedWidget->setEnabled(false);

    showProgress();
}

void LoginDialog::emitAccept()
{
    //
    // Эмулируем нажатия кнопок Accept
    //
    if (m_ui->tabs->currentIndex() == 0) {
        emit m_ui->loginButtons->accepted();
    } else if (!m_isVerify) {
        emit m_ui->signUpButtons->accepted();
    }
}

void LoginDialog::initView()
{
    m_ui->loginError->clear();
    m_ui->loginErrorIcon->clear();

    m_ui->signUpError->clear();
    m_ui->signUpErrorIcon->clear();

    m_ui->verificationError->clear();
    m_ui->verificationErrorIcon->clear();

    //
    // Красивые табы
    //
    m_ui->tabs->addTab(tr("Login"));
    m_ui->tabs->addTab(tr("Sign Up"));
    m_ui->tabs->setProperty("inTopPanel", true);

    QWidget::setTabOrder(m_ui->loginEmail, m_ui->loginPasswordEdit);
    QWidget::setTabOrder(m_ui->signUpEmail, m_ui->signUpPasswordEdit);
}

void LoginDialog::initConnections()
{
    connect(this, &LoginDialog::accepted, this, &LoginDialog::emitAccept);
    connect(this, &LoginDialog::rejected, this, &LoginDialog::hide);

    connect(m_ui->loginButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::tryLogin);
    connect(m_ui->signUpButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::trySignUp);
    connect(m_ui->loginButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);
    connect(m_ui->signUpButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(m_ui->buttonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::cancelVerify);
    connect(m_ui->buttonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(m_ui->restorePassword, &QPushButton::clicked, [this] {
        if (m_ui->loginEmail->text().isEmpty()) {
            setLoginError(tr("Email is empty"));
        } else {
            //
            // Обязательно в таком порядке, иначе возможен бесконечный блок
            //
            block();
            emit restoreRequested();
        }
    });

    connect(m_ui->verificationCode, &QLineEdit::textChanged,
            this, &LoginDialog::checkVerificationCode);

    connect(m_ui->tabs, &TabBarExpanded::currentChanged,
            this, &LoginDialog::switchWidget);
}

void LoginDialog::clear()
{
    m_ui->loginEmail->clear();
    m_ui->loginPasswordEdit->clear();
    m_ui->signUpEmail->clear();
    m_ui->signUpPasswordEdit->clear();
    m_ui->verificationError->clear();

    m_ui->tabs->setCurrentIndex(0);
    switchWidget();

    m_isVerify = false;

    m_ui->loginError->clear();
    m_ui->signUpError->clear();
    m_ui->verificationError->clear();

    m_ui->loginErrorIcon->clear();
    m_ui->signUpErrorIcon->clear();
    m_ui->verificationErrorIcon->clear();

    m_ui->restorePassword->setEnabled(true);
}

void LoginDialog::updateLabel(QLabel *_label, QLabel* _icon,
                              const QString &_message, bool _isError)
{
    _label->setText(_message);
    _icon->setPixmap(_icon->style()->
                      standardIcon(_isError? QStyle::SP_MessageBoxCritical
                                           : QStyle::SP_MessageBoxInformation).
                      pixmap(ICON_PIXMAP_SIZE));

}
