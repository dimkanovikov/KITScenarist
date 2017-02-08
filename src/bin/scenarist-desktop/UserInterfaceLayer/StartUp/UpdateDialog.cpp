#include "UpdateDialog.h"
#include "ui_UpdateDialog.h"

using UserInterface::UpdateDialog;

UpdateDialog::UpdateDialog(QWidget *parent) :
    QLightBoxDialog(parent),
    m_ui(new Ui::UpdateDialog)
{
    m_ui->setupUi(this);

    initView();
    initConnections();
    initStylesheet();
}

UpdateDialog::~UpdateDialog()
{
    delete m_ui;
}

int UpdateDialog::showUpdate(const QString &_version, const QString &_text,
                             bool _isBeta)
{
    setWindowTitle(QString("New version %1 is available").arg(_version));
    m_ui->text->setText(_text);
    m_ui->betaAgree->setVisible(_isBeta);
    m_ui->whatIsBeta->setVisible(_isBeta);
    m_ui->download->setEnabled(!_isBeta);

    return exec();
}

void UpdateDialog::downloadFinished()
{
    m_downloaded = true;
    hideProgress();
    m_ui->download->setEnabled(true);
    m_ui->download->setText(tr("Install"));
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
