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

#include "UpdateDialog.h"
#include "ui_UpdateDialog.h"

using UserInterface::UpdateDialog;

UpdateDialog::UpdateDialog(QWidget *parent) :
    QLightBoxDialog(parent),
    m_ui(new Ui::UpdateDialog)
{
    m_ui->setupUi(this);

    initStylesheet();
}

UpdateDialog::~UpdateDialog()
{
    delete m_ui;
}

int UpdateDialog::showUpdate(const QString &_version, const QString &_text,
                             bool _isBeta, bool _isSupported)
{
    setWindowTitle(tr("New version %1 is available").arg(_version));
    m_ui->text->setText(_text);
    m_ui->betaAgree->setVisible(_isBeta && _isSupported);
    m_ui->whatIsBeta->setVisible(_isBeta && _isSupported);
    m_ui->notSupportedLabel->setVisible(!_isSupported);
    m_ui->download->setText(tr("Download"));
    m_ui->download->setEnabled((!_isBeta && _isSupported) || m_ui->betaAgree->isChecked());
    m_downloaded = false;

    return exec();
}

void UpdateDialog::downloadFinished()
{
    m_downloaded = true;
    hideProgress();
    m_ui->download->setEnabled(true);
    m_ui->download->setText(tr("Install"));
}

void UpdateDialog::showDownloadError()
{
    m_ui->text->setText(tr("Error during download update.\n\nYou can try to reload update."));
    hideProgress();
    m_ui->download->setEnabled(true);
}

void UpdateDialog::initView()
{
    m_ui->text->setOpenExternalLinks(true);

    QLightBoxDialog::initView();
}

void UpdateDialog::initConnections()
{
    connect(m_ui->cancel, &QPushButton::clicked, this, &UpdateDialog::reject);
    connect(m_ui->skip, &QPushButton::clicked, this, &UpdateDialog::skipUpdate);
    connect(m_ui->skip, &QPushButton::clicked, this, &UpdateDialog::reject);
    connect(m_ui->betaAgree, &QCheckBox::clicked, m_ui->download, &QPushButton::setEnabled);
    connect(m_ui->download, &QPushButton::clicked, [this] {
        if (m_downloaded) {
            emit accept();
        } else {
            showProgress(0, 100);
            m_ui->download->setEnabled(false);
            emit downloadUpdate();
        }
    });

    QLightBoxDialog::initConnections();
}

void UpdateDialog::initStylesheet()
{
    m_ui->text->setStyleSheet("background-color: palette(window);"
                              "border: none;");
}
