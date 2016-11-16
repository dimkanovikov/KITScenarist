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

#include "RenewSubscriptionDialog.h"
#include "ui_RenewSubscriptionDialog.h"

#include <QPushButton>

using UserInterface::RenewSubscriptionDialog;

RenewSubscriptionDialog::RenewSubscriptionDialog(QWidget *_parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::RenewSubscriptionDialog)
{
    m_ui->setupUi(this);

    m_ui->buttonBox->addButton(new QPushButton(tr("Renew")), QDialogButtonBox::AcceptRole);

    m_ui->duration->addItem(tr("1 month"), 1);
    m_ui->duration->addItem(tr("2 month"), 2);
    m_ui->duration->addItem(tr("3 month"), 3);
    m_ui->duration->addItem(tr("6 month (6% discount)"), 6);
    m_ui->duration->addItem(tr("12 month (12% discount)"), 12);

    setPaymentText();

    connect(m_ui->duration, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &RenewSubscriptionDialog::setPaymentText);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &RenewSubscriptionDialog::accept);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected, this, &RenewSubscriptionDialog::reject);

    QLightBoxDialog::initView();
}

RenewSubscriptionDialog::~RenewSubscriptionDialog()
{
    delete m_ui;
}

unsigned RenewSubscriptionDialog::duration() const
{
    return m_ui->duration->currentData().toInt();
}

unsigned RenewSubscriptionDialog::amount() const
{
    return durationToAmount(duration());
}

unsigned RenewSubscriptionDialog::paymentSystemType() const
{
    return m_ui->paymentType->currentIndex();
}

void RenewSubscriptionDialog::setPaymentText()
{
    m_ui->payment->setText(tr("for %1 rubles").arg(amount()));
}

unsigned RenewSubscriptionDialog::durationToAmount(unsigned _duration) const
{
    //
    // 1 месяц - 299 рублей
    //
    unsigned amount = _duration * 299;

    //
    // Применим скидку, если число месяцев больше 5
    //
    if(_duration > 5) {
        amount *= 1 - _duration / 100;
    }
}
