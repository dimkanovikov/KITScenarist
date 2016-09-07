#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include <QRegExpValidator>

using UserInterface::LoginDialog;

LoginDialog::LoginDialog(QWidget* _parent) :
	QLightBoxDialog(_parent),
    ui(new Ui::LoginDialog)
{
	ui->setupUi(this);

	initView();
	initConnections();


    isVerify = false;
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

QString LoginDialog::loginEmail() const
{
    return ui->loginEmail->text();
}

QString LoginDialog::signUpEmail() const
{
    return ui->signUpEmail->text();
}

QString LoginDialog::loginPassword() const
{
    return ui->loginPasswordEdit->text();
}

QString LoginDialog::signUpPassword() const
{
    return ui->signUpPasswordEdit->text();
}

void LoginDialog::setLoginError(const QString& _error)
{
    ui->loginError->setStyleSheet("QLabel { color : red; }");
    ui->loginError->setText(_error);
    ui->loginError->show();
    unblock();
}

void LoginDialog::setSignUpError(const QString &_error)
{
    ui->signUpError->setText(_error);
    ui->signUpError->show();
    unblock();
}

void LoginDialog::setVerificationError(const QString &_error)
{
    ui->verificationError->setStyleSheet("QLabel { color : red; }");
    ui->verificationError->setText(_error);
    ui->verificationError->show();
    unblock();
}

QString LoginDialog::signUpType() const
{
    if (ui->startButton->isChecked()) {
        return tr("start");
    } else if (ui->basicButton->isChecked()) {
        return tr("basic");
    } else if (ui->maximalButton->isChecked()) {
        return tr("maximal");
    } else return tr("unexpected");
}

QString LoginDialog::verificationCode() const
{
    return ui->verificationCode->text();
}

void LoginDialog::checkCode()
{
    QRegExpValidator validator(QRegExp("[0-9]{5}"));
    QString s = ui->verificationCode->text();
    int pos = 0;
    if (validator.validate(s, pos) == QValidator::Acceptable) {
        block();
        emit verify();
    }
}

void LoginDialog::showVerification()
{
    isVerify = true;
    ui->verificationError->setStyleSheet("QLabel { color : green; }");
    ui->verificationError->setText(tr("your e-mail \"%1\" was sent a letter "
                                      "with a confirmation code").arg(ui->signUpEmail->text()));
    ui->verificationError->show();
    ui->stackedWidget->setCurrentWidget(ui->verificationPage);

    unblock();
    ui->verificationCode->setFocus();
}

void LoginDialog::showRestore()
{
    ui->loginError->setStyleSheet("QLabel { color : green; }");
    ui->loginError->setText(tr("your e-mail \"%1\" was sent a letter "
                                      "with a password").arg(ui->loginEmail->text()));
    ui->restorePassword->hide();
    ui->loginError->show();

    unblock();
}

void LoginDialog::setLoginPage()
{
    isVerify = false;
    ui->tabs->setCurrentIndex(0);
}

void LoginDialog::clear()
{
    ui->loginEmail->clear();
    ui->loginPasswordEdit->clear();
    ui->signUpEmail->clear();
    ui->signUpPasswordEdit->clear();
    ui->verificationError->clear();

    ui->tabs->setCurrentIndex(0);
    ui->basicButton->setChecked(true);
    switchWidget();

    isVerify = false;

    ui->loginError->clear();
    ui->signUpError->clear();
    ui->verificationError->clear();

    ui->restorePassword->show();

}

QWidget* LoginDialog::focusedOnExec() const
{
    return ui->loginEmail;
}

void LoginDialog::initView()
{

    ui->loginError->hide();
    ui->loginButtons->addButton(tr("Login"), QDialogButtonBox::AcceptRole);

    ui->signUpError->hide();
    ui->signUpButtons->addButton(tr("Sign Up"), QDialogButtonBox::AcceptRole);

    ui->verificationError->hide();

    //
    // Красивые табы
    //
    ui->tabs->addTab(tr("Login"));
    ui->tabs->addTab(tr("Sign Up"));
    ui->tabs->setProperty("inTopPanel", true);

    QLightBoxDialog::initView();

    QWidget::setTabOrder(ui->loginEmail, ui->loginPasswordEdit);
    QWidget::setTabOrder(ui->signUpEmail, ui->signUpPasswordEdit);
}

void LoginDialog::initConnections()
{
    connect(this, &LoginDialog::accepted, this, &LoginDialog::needAccept);
    connect(this, &LoginDialog::rejected, this, &LoginDialog::hide);

    connect(ui->loginButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::block);
    connect(ui->loginButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::login);
    connect(ui->loginButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(ui->signUpButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::block);
    connect(ui->signUpButtons, &QDialogButtonBox::accepted,
            this, &LoginDialog::signUp);
    connect(ui->signUpButtons, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(ui->ButtonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::cancelVerify);
    connect(ui->ButtonsVerification, &QDialogButtonBox::rejected,
            this, &LoginDialog::hide);

    connect(ui->restorePassword, &QPushButton::clicked,
            this, &LoginDialog::block);
    connect(ui->restorePassword, &QPushButton::clicked,
            this, &LoginDialog::restore);

    connect(ui->verificationCode, &QLineEdit::textChanged,
            this, &LoginDialog::checkCode);

    connect(ui->tabs, &TabBarExpanded::currentChanged,
            this, &LoginDialog::switchWidget);;

	QLightBoxDialog::initConnections();
}

void LoginDialog::show()
{
    clear();
    QLightBoxDialog::show();
    ui->loginEmail->setFocus();
}

void LoginDialog::needAccept()
{
    //
    // Эмулируем нажатия кнопок Accept
    if (ui->tabs->currentIndex() == 0) {
        emit ui->loginButtons->accepted();
    }
    else if (!isVerify) {
        emit ui->signUpButtons->accepted();
    }
}

void LoginDialog::block()
{
    ui->tabs->setEnabled(false);

    ui->stackedWidget->setEnabled(false);

    showProgress();
}

void LoginDialog::unblock()
{
    ui->tabs->setEnabled(true);

    ui->stackedWidget->setEnabled(true);

    hideProgress();
}

void LoginDialog::cancelVerify()
{
    isVerify = false;
    switchWidget();
}

void LoginDialog::switchWidget()
{
    if (ui->tabs->currentIndex() == 0) {
        ui->stackedWidget->setCurrentWidget(ui->loginPage);
        ui->loginEmail->setFocus();
    }
    else if (isVerify) {
        ui->stackedWidget->setCurrentWidget(ui->verificationPage);
        ui->verificationCode->setFocus();
    }
    else {
        ui->stackedWidget->setCurrentWidget(ui->signUpPage);
        ui->signUpEmail->setFocus();
    }
}
