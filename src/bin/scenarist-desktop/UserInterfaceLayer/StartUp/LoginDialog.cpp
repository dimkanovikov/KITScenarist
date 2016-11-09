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

#include <QRegExpValidator>

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

	initView();
    initConnections();
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

void LoginDialog::showVerificationSuccess()
{
    m_isVerify = true;
    updateLabel(m_ui->verificationError, m_ui->verificationErrorIcon,
                tr("your e-mail \"%1\" was sent a letter with a "
                   "confirmation code").arg(m_ui->signUpEmail->text()), false);
    m_ui->stackedWidget->setCurrentWidget(m_ui->verificationPage);

    unblock();
    m_ui->verificationCode->setFocus();
}

void LoginDialog::showRestoreSuccess()
{
    updateLabel(m_ui->loginError, m_ui->loginErrorIcon,
                tr("your e-mail \"%1\" was sent a letter with a password").
                arg(m_ui->loginEmail->text()),
                  false);
    m_ui->restorePassword->hide();

    unblock();
}

void LoginDialog::showPrepared()
{
    clear();
    QLightBoxDialog::show();
    m_ui->loginEmail->setFocus();
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

void LoginDialog::loginAcceptButton()
{
    if (abstractAcceptButton(m_ui->loginEmail, m_ui->loginError, m_ui->loginErrorIcon)) {
        emit loginRequested();
    }
}

void LoginDialog::signUpAcceptButton()
{
    if (abstractAcceptButton(m_ui->signUpEmail, m_ui->signUpError, m_ui->signUpErrorIcon)) {
        emit signUpRequested();
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

void LoginDialog::unblock()
{
    m_ui->tabs->setEnabled(true);

    m_ui->stackedWidget->setEnabled(true);

    hideProgress();
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
    m_loginButton = m_ui->loginButtons->addButton(tr("Login"), QDialogButtonBox::AcceptRole);

    m_ui->signUpError->clear();
    m_ui->signUpErrorIcon->clear();
    m_signUpButton = m_ui->signUpButtons->addButton(tr("Sign Up"), QDialogButtonBox::AcceptRole);

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

    QLightBoxDialog::initView();
}

void LoginDialog::initConnections()
{
    connect(this, &LoginDialog::accepted, this, &LoginDialog::emitAccept);
    connect(this, &LoginDialog::rejected, this, &LoginDialog::hide);

    connect(m_ui->loginButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::loginAcceptButton);
    connect(m_ui->signUpButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::signUpAcceptButton);
    connect(m_ui->loginButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);
    connect(m_ui->signUpButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(m_ui->buttonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::cancelVerify);
    connect(m_ui->buttonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(m_ui->restorePassword, &QPushButton::clicked,
            this, &LoginDialog::block);
    connect(m_ui->restorePassword, &QPushButton::clicked,
            this, &LoginDialog::restoreRequested);

    connect(m_ui->verificationCode, &QLineEdit::textChanged,
            this, &LoginDialog::checkVerificationCode);

    connect(m_ui->tabs, &TabBarExpanded::currentChanged,
            this, &LoginDialog::switchWidget);;

	QLightBoxDialog::initConnections();
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

    m_ui->restorePassword->show();
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

bool LoginDialog::abstractAcceptButton(QLineEdit *_line, QLabel* _label, QLabel* _icon)
{
    if (checkEmailValidy(_line->text())) {
        _label->clear();
        _icon->clear();
        block();
        return true;
    } else {
        updateLabel(_label, _icon, tr("Email is invalid"), true);
        return false;
    }
}

bool LoginDialog::checkEmailValidy(const QString& _email)
{
    //
    // Для валидатора нужна неконстантная ссылка,
    // поэтому копируем
    //
    QString toCheck = _email;

    QRegExpValidator validator(QRegExp(".+@.{2,}\\..{2,}"));
    int pos = 0;
    if (validator.validate(toCheck, pos) != QValidator::Acceptable) {
        return false;
    } else {
        return true;
    }

}
