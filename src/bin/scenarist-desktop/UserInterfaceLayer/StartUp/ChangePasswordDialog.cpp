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

    connect(m_ui->oldPassword, &PasswordLineEdit::textChanged,
            this, &ChangePasswordDialog::dataChanged);
    connect(m_ui->newPassword, &PasswordLineEdit::textChanged,
            this, &ChangePasswordDialog::dataChanged);


    dataChanged();

    QLightBoxDialog::initView();
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
    show();
    m_ui->oldPassword->setFocus();
}

QString ChangePasswordDialog::getPassword() const
{
    return m_ui->oldPassword->text();
}

QString ChangePasswordDialog::getNewPassword() const
{
    return m_ui->newPassword->text();
}

void ChangePasswordDialog::hide()
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
    //
    // Соединения к сигналу accepted должны быть именно в таком порядке
    // иначе, пароль изменится, окно разблокируется, а затем заблокируется
    //
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &ChangePasswordDialog::block);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &ChangePasswordDialog::changeRequested);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected,
            this, &ChangePasswordDialog::hide);

    QLightBoxDialog::initConnections();
}

void ChangePasswordDialog::dataChanged()
{
    if(m_ui->oldPassword->text().isEmpty() || m_ui->newPassword->text().isEmpty()) {
        m_accept->setEnabled(false);
    } else {
        m_accept->setEnabled(true);
    }
}
