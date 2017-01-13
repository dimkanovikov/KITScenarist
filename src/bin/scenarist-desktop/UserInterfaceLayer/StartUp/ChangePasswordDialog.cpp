/*
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

#include "ChangePasswordDialog.h"
#include "ui_ChangePasswordDialog.h"

#include <QPushButton>

using UserInterface::ChangePasswordDialog;

ChangePasswordDialog::ChangePasswordDialog(QWidget *_parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::ChangePasswordDialog),
    m_accept(new QPushButton(tr("Change")))
{
    m_ui->setupUi(this);
    m_ui->buttonBox->addButton(m_accept, QDialogButtonBox::AcceptRole);

    setAcceptButtonAvailability();

    initView();
    initConnections();
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete m_ui;
}

void ChangePasswordDialog::showPrepared()
{
    m_ui->oldPassword->clear();
    m_ui->newPassword->clear();
    m_ui->oldPassword->resetAsterisk();
    m_ui->newPassword->resetAsterisk();
    show();
    m_ui->oldPassword->setFocus();
}

void ChangePasswordDialog::showUnprepared()
{
    show();
    m_ui->oldPassword->setFocus();
}

QString ChangePasswordDialog::password() const
{
    return m_ui->oldPassword->text();
}

QString ChangePasswordDialog::newPassword() const
{
    return m_ui->newPassword->text();
}

void ChangePasswordDialog::stopAndHide()
{
    QLightBoxDialog::setEnabled(true);

    hideProgress();

    QLightBoxDialog::hide();
}

void ChangePasswordDialog::block()
{
    QLightBoxDialog::setEnabled(false);

    showProgress();
}

void ChangePasswordDialog::initConnections()
{
    connect(this, &ChangePasswordDialog::accepted, [this] {
        if (m_accept->isEnabled()) {
            emit m_ui->buttonBox->accepted();
        }
    });
    connect(this, &ChangePasswordDialog::rejected,
            this, &ChangePasswordDialog::hide);
    connect(m_ui->oldPassword, &PasswordLineEdit::textChanged,
            this, &ChangePasswordDialog::setAcceptButtonAvailability);
    connect(m_ui->newPassword, &PasswordLineEdit::textChanged,
            this, &ChangePasswordDialog::setAcceptButtonAvailability);

    //
    // Соединения к сигналу accepted должны быть именно в таком порядке
    // иначе, пароль изменится, окно разблокируется, а затем заблокируется
    //
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &ChangePasswordDialog::block);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &ChangePasswordDialog::changeRequested);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected,
            this, &ChangePasswordDialog::stopAndHide);

    QWidget::setTabOrder(m_ui->oldPassword, m_ui->newPassword);
    QLightBoxDialog::initConnections();
}

void ChangePasswordDialog::initView()
{
    QLightBoxDialog::initView();
}

void ChangePasswordDialog::setAcceptButtonAvailability()
{
    if(m_ui->oldPassword->text().isEmpty() || m_ui->newPassword->text().isEmpty()) {
        m_accept->setEnabled(false);
    } else {
        m_accept->setEnabled(true);
    }
}
