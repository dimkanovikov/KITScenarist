#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include <QRegExpValidator>

using UserInterface::LoginDialog;

LoginDialog::LoginDialog(QWidget* _parent) :
	QLightBoxDialog(_parent),
    ui(new Ui::LoginDialog)
{
    m_ui->setupUi(this);

	initView();
	initConnections();

    m_isVerify = false;
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

QString LoginDialog::signUpType() const
{
    if (m_ui->startButton->isChecked()) {
        return tr("start");
    } else if (m_ui->basicButton->isChecked()) {
        return tr("basic");
    } else if (m_ui->maximalButton->isChecked()) {
        return tr("maximal");
    } else return tr("unexpected");
}

void LoginDialog::setLoginError(const QString& _error)
{
    m_ui->loginError->setStyleSheet("QLabel { color : red; }");
    m_ui->loginError->setText(_error);
    unblock();
}

void LoginDialog::setSignUpError(const QString &_error)
{
    m_ui->signUpError->setText(_error);
    unblock();
}

void LoginDialog::setVerificationError(const QString &_error)
{
    m_ui->verificationError->setStyleSheet("QLabel { color : red; }");
    m_ui->verificationError->setText(_error);
    unblock();
}

void LoginDialog::showVerification()
{
    m_isVerify = true;
    m_ui->verificationError->setStyleSheet("QLabel { color : green; }");
    m_ui->verificationError->setText(tr("your e-mail \"%1\" was sent a letter "
                                      "with a confirmation code").arg(m_ui->signUpEmail->text()));
    m_ui->stackedWidget->setCurrentWidget(m_ui->verificationPage);

    unblock();
    m_ui->verificationCode->setFocus();
}

void LoginDialog::showRestore()
{
    m_ui->loginError->setStyleSheet("QLabel { color : green; }");
    m_ui->loginError->setText(tr("your e-mail \"%1\" was sent a letter "
                                      "with a password").arg(m_ui->loginEmail->text()));
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

void LoginDialog::checkCode()
{
    QRegExpValidator validator(QRegExp("[0-9]{5}"));
    QString s = m_ui->verificationCode->text();
    int pos = 0;
    if (validator.validate(s, pos) == QValidator::Acceptable) {
        block();
        emit verify();
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

void LoginDialog::needAccept()
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
    m_ui->loginButtons->addButton(tr("Login"), QDialogButtonBox::AcceptRole);

    m_ui->signUpError->clear();
    m_ui->signUpButtons->addButton(tr("Sign Up"), QDialogButtonBox::AcceptRole);

    m_ui->verificationError->clear();

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
    connect(this, &LoginDialog::accepted, this, &LoginDialog::needAccept);
    connect(this, &LoginDialog::rejected, this, &LoginDialog::hide);

    connect(m_ui->loginButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::block);
    connect(m_ui->loginButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::login);
    connect(m_ui->loginButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(m_ui->signUpButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::block);
    connect(m_ui->signUpButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::signUp);
    connect(m_ui->signUpButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(m_ui->buttonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::cancelVerify);
    connect(m_ui->buttonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(m_ui->restorePassword, &QPushButton::clicked,
            this, &LoginDialog::block);
    connect(m_ui->restorePassword, &QPushButton::clicked,
            this, &LoginDialog::restore);

    connect(m_ui->verificationCode, &QLineEdit::textChanged,
            this, &LoginDialog::checkCode);

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
    m_ui->basicButton->setChecked(true);
    switchWidget();

    m_isVerify = false;

    m_ui->loginError->clear();
    m_ui->signUpError->clear();
    m_ui->verificationError->clear();

    m_ui->restorePassword->show();
}
