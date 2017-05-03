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
#include <QTimer>

using UserInterface::RenewSubscriptionDialog;

namespace {
    const int bankCardPayment = 0;
    const int oneMonth = 0;
}

RenewSubscriptionDialog::RenewSubscriptionDialog(QWidget *_parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::RenewSubscriptionDialog)
{
    m_ui->setupUi(this);
    m_acceptButton = m_ui->buttonBox->addButton(tr("Renew"), QDialogButtonBox::AcceptRole);
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

void RenewSubscriptionDialog::showPrepared()
{
    m_ui->stackedWidget->setCurrentWidget(m_ui->settingsPage);
    m_ui->paymentType->setCurrentIndex(bankCardPayment);
    m_ui->duration->setCurrentIndex(oneMonth);
    m_acceptButton->show();
    m_ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    setWindowWaiting(false);

    setPaymentText();

    show();
}

void RenewSubscriptionDialog::showThanks(const QString &_expDate)
{
    setWindowWaiting(false);
    m_ui->subscriptionDate->setText(_expDate);
    m_ui->stackedWidget->setCurrentWidget(m_ui->thanksPage);
    m_acceptButton->hide();
    m_ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));
}

void RenewSubscriptionDialog::setPaymentMonth(int _paymentMonth)
{
    m_paymentMonth = _paymentMonth;
}

void RenewSubscriptionDialog::setPaymentText()
{
    m_ui->payment->setText(tr("for %1 rubles").arg(amount()));
}

void RenewSubscriptionDialog::setWindowWaiting(bool _isWaiting)
{
    m_ui->duration->setEnabled(!_isWaiting);
    m_ui->paymentType->setEnabled(!_isWaiting);
    m_acceptButton->setEnabled(!_isWaiting);

    if (!_isWaiting) {
        QLightBoxDialog::hideProgress();
    } else {
        QLightBoxDialog::showProgress();
    }
}

unsigned RenewSubscriptionDialog::durationToAmount(unsigned _duration) const
{
    unsigned amount = _duration * m_paymentMonth;

    //
    // Применим скидку, если число месяцев больше 5
    //
    if(_duration > 5) {
        amount *= 1.0 - static_cast<double>(_duration) / 100;
    }
    return amount;
}

void RenewSubscriptionDialog::initView()
{
    m_ui->duration->addItem(tr("1 month"), 1);
    m_ui->duration->addItem(tr("2 month"), 2);
    m_ui->duration->addItem(tr("3 month"), 3);
    m_ui->duration->addItem(tr("6 month (6% discount)"), 6);
    m_ui->duration->addItem(tr("12 month (12% discount)"), 12);
    m_ui->duration->setCurrentIndex(0);
}

void RenewSubscriptionDialog::initConnections()
{
    connect(m_ui->duration, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &RenewSubscriptionDialog::setPaymentText);
    //connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &RenewSubscriptionDialog::accept);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &RenewSubscriptionDialog::renewSubsciptionRequested);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, [this] {
        setWindowWaiting(true);
    });

    connect(m_ui->buttonBox, &QDialogButtonBox::rejected, this, &RenewSubscriptionDialog::reject);
    connect(this, &RenewSubscriptionDialog::rejected, this, &RenewSubscriptionDialog::hide);
}
