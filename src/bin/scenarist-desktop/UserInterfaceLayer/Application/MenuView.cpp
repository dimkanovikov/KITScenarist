#include "MenuView.h"
#include "ui_MenuView.h"

#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.h>

#include <QDesktopServices>
#include <QMenu>
#include <QTimer>
#include <QUrl>

using UserInterface::MenuView;

namespace {
    /**
     * @brief Номера пунктов меню
     */
    /** @{ */
    const int SAVE_MENU_INDEX = 2;
    const int SAVE_AS_MENU_INDEX = 3;
    const int IMPORT_MENU_INDEX = 5;
    const int EXPORT_MENU_INDEX = 6;
    const int PRINT_PREVIEW_MENU_INDEX = 7;
    /** @} */

    /**
     * @brief Получить язык для подстановки в ссылки на сайте
     */
    static QString urlLanguage() {
        switch (QLocale().language()) {
            case QLocale::Russian:
            case QLocale::Ukrainian:
            case QLocale::Kazakh: {
                return QString();
            }

            default: {
                return "en/";
            }
        }
    }
}


MenuButton::MenuButton(QWidget* _parent) :
    QPushButton(_parent),
    m_shortcutText(new QLabel(this))
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->addWidget(m_shortcutText);
}

void MenuButton::setShortcutText(const QString& _text)
{
    m_shortcutText->setText(_text);
}


MenuView::MenuView(QWidget* _parent) :
    QWidget(_parent),
    m_ui(new Ui::MenuView)
{
    m_ui->setupUi(this);

    initView();
    initMenuButtons();
    initConnections();
    initStyleSheet();
}

MenuView::~MenuView()
{
    delete m_ui;
}

void MenuView::setMenu(QMenu* _menu)
{
    if (m_menu != _menu) {
        m_menu = _menu;

        for (int menuButtonIndex = 0; menuButtonIndex < m_menuButtons.size(); ++menuButtonIndex) {
            MenuButton* menuButton = m_menuButtons[menuButtonIndex];
            QAction* menuAction = m_menu->actions()[menuButtonIndex];
            if (menuButton != nullptr) {
                menuButton->setShortcutText(menuAction->shortcut().toString(QKeySequence::NativeText));
                connect(menuButton, &QPushButton::clicked, this, &MenuView::hideRequested);
                connect(menuButton, &QPushButton::clicked, menuAction, &QAction::trigger);
            }
        }
    }
}

QMenu* MenuView::menu() const
{
    return m_menu;
}

void MenuView::setMenuItemEnabled(int _index, bool _enabled)
{
    m_menu->actions()[_index]->setEnabled(_enabled);
    if (m_menuButtons.value(_index, nullptr) != nullptr) {
        m_menuButtons[_index]->setEnabled(_enabled);
    }
}

void MenuView::disableProjectActions()
{
    const bool disabled = false;
    setMenuItemEnabled(SAVE_MENU_INDEX, disabled);
    setMenuItemEnabled(SAVE_AS_MENU_INDEX, disabled);
    setMenuItemEnabled(IMPORT_MENU_INDEX, disabled);
    setMenuItemEnabled(EXPORT_MENU_INDEX, disabled);
    setMenuItemEnabled(PRINT_PREVIEW_MENU_INDEX, disabled);
}

void MenuView::enableProjectActions()
{
    const bool enabled = true;
    setMenuItemEnabled(SAVE_MENU_INDEX, enabled);
    setMenuItemEnabled(SAVE_AS_MENU_INDEX, enabled);
    setMenuItemEnabled(IMPORT_MENU_INDEX, enabled);
    setMenuItemEnabled(EXPORT_MENU_INDEX, enabled);
    setMenuItemEnabled(PRINT_PREVIEW_MENU_INDEX, enabled);
}

void MenuView::enableProgressLoginLabel(int _dots, bool _firstUpdate)
{
    QApplication::processEvents();

    if (_firstUpdate) {
        m_isProcessLogin = true;
    }

    if (m_isProcessLogin) {
        m_ui->login->setText(tr("Connect") + QString(".").repeated(_dots));
        QTimer::singleShot(1000, Qt::VeryCoarseTimer, [this, _dots] {
            enableProgressLoginLabel((_dots + 1) % 4, false);
        });
    }
}

void MenuView::disableProgressLoginLabel()
{
    m_isProcessLogin = false;
    m_ui->login->setText(tr("<a href=\"#\" style=\"color:#2b78da;\">Login</a>"));
}

void MenuView::setUserLogged(bool _isLogged, const QString& _userName, const QString& _email)
{
    m_ui->login->setVisible(!_isLogged);
    m_ui->loginInfo->setVisible(!_isLogged);

    m_ui->account->setVisible(_isLogged);
    m_ui->account->setText(_userName);
    m_ui->email->setVisible(_isLogged);
    m_ui->email->setText(_email);

    m_ui->userName->setAcceptedText(_userName);

    if (!_isLogged) {
        showMenuPage();
    }
}

void MenuView::setSubscriptionInfo(bool _isActive, const QString& _expDate, quint64 _usedSpace, quint64 _availableSpace)
{
    if (_isActive) {
        m_ui->subscriptionActivity->setText(tr("Account is active until:"));
        m_ui->subscriptionEndDate->setText(_expDate);
    } else {
        m_ui->subscriptionActivity->setText(tr("Account is inactive"));
        m_ui->subscriptionEndDate->clear();
    }

    //
    // Делаем вид, что пользователь не может использовать больше, чем ему доступно
    //
    if (_usedSpace > _availableSpace) {
        _usedSpace = _availableSpace;
    }
    m_ui->availableSpaceInfo->setText(
                tr("Used %1 Gb from %2 Gb")
                .arg(QString::number(qreal(_usedSpace / 1000000000.), 'f', 2))
                .arg(QString::number(qreal(_availableSpace / 1000000000.), 'f', 2)));
    m_ui->availableSpaceProgress->setMaximum(_availableSpace / 1000000);
    m_ui->availableSpaceProgress->setValue(_usedSpace / 1000000);
}

void MenuView::showMenuPage(bool _animate)
{
    if (_animate) {
        WAF::StackedWidgetAnimation::slide(m_ui->menuContent, m_ui->menuActions, WAF::FromLeftToRight);
    } else {
        m_ui->menuContent->setCurrentWidget(m_ui->menuActions);
    }
}

void MenuView::showAccountPage()
{
    WAF::StackedWidgetAnimation::slide(m_ui->menuContent, m_ui->menuAccount, WAF::FromRightToLeft);
}

bool MenuView::event(QEvent* _event)
{
    if (_event->type() == QEvent::PaletteChange) {
        initMenuButtons();
    }

    return QWidget::event(_event);
}

void MenuView::initView()
{
    m_ui->avatar->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_ui->login->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_ui->loginInfo->setAttribute(Qt::WA_TransparentForMouseEvents);

    m_ui->menuContent->setCurrentWidget(m_ui->menuActions);

    m_menuButtons << m_ui->createProject
                  << m_ui->openProject
                  << m_ui->saveProject
                  << m_ui->saveProjectAs
                  << nullptr // тут у оригинального меню разделитель
                  << m_ui->importProject
                  << m_ui->exportProject
                  << m_ui->printPreview;

    m_ui->version->setText(QApplication::applicationVersion());
}

void MenuView::initMenuButtons()
{
    auto updateButton = [this] (QAbstractButton* _button) {
        const QString text = _button->text().simplified();
        _button->setText("   " + text);

        QIcon icon = _button->icon();
        ImageHelper::setIconColor(icon, palette().text().color());
        _button->setIcon(icon);
    };

    updateButton(m_ui->createProject);
    updateButton(m_ui->openProject);
    updateButton(m_ui->saveProject);
    updateButton(m_ui->saveProjectAs);
    updateButton(m_ui->importProject);
    updateButton(m_ui->exportProject);
    updateButton(m_ui->printPreview);
    updateButton(m_ui->help);

    updateButton(m_ui->getSubscriptionInfo);
    updateButton(m_ui->renewSubscription);
    updateButton(m_ui->changePassword);
    updateButton(m_ui->logout);
}

void MenuView::initConnections()
{
    connect(m_ui->accountPanel, &ClickableFrame::clicked, [this] {
        //
        // Если пользователь не авторизован, отправим запрос на авторизацию
        //
        if (m_ui->login->isVisible()) {
            emit loginPressed();
        }
        //
        // А если авторизован, то покажем меню или личный кабинет
        //
        else {
            if (m_ui->menuContent->currentWidget() == m_ui->menuActions) {
                showAccountPage();
            } else {
                showMenuPage();
            }
        }
    });

    connect(m_ui->userName, &AcceptebleLineEdit::textAccepted, this, &MenuView::userNameChanged);
    connect(m_ui->changePassword, &QPushButton::clicked, this, &MenuView::passwordChangeClicked);
    connect(m_ui->getSubscriptionInfo, &QToolButton::clicked, this, &MenuView::getSubscriptionInfoClicked);
    connect(m_ui->renewSubscription, &QPushButton::clicked, this, &MenuView::renewSubscriptionClicked);
    connect(m_ui->logout, &QPushButton::clicked, this, &MenuView::logoutClicked);

    connect(m_ui->help, &QPushButton::clicked, [this] {
        const QString url = QString("https://kitscenarist.ru/%1help/").arg(urlLanguage());
        QDesktopServices::openUrl(QUrl(url));
    });

    connect(m_ui->appUrl, &ClickableLabel::clicked, [this] {
        const QString url = QString("https://kitscenarist.ru/%1").arg(urlLanguage());
        QDesktopServices::openUrl(QUrl(url));
    });
    connect(m_ui->aboutApp, &ClickableLabel::clicked, this, &MenuView::aboutAppPressed);
}

void MenuView::initStyleSheet()
{
    m_ui->accountPanel->setProperty("menuAccount", true);
    m_ui->login->setProperty("link", true);
    m_ui->account->setProperty("link", true);

    m_ui->createProject->setProperty("menuButton", true);
    m_ui->openProject->setProperty("menuButton", true);
    m_ui->saveProject->setProperty("menuButton", true);
    m_ui->saveProjectAs->setProperty("menuButton", true);
    m_ui->importProject->setProperty("menuButton", true);
    m_ui->importProject->setProperty("menuButtonTopBordered", true);
    m_ui->exportProject->setProperty("menuButton", true);
    m_ui->printPreview->setProperty("menuButton", true);
    m_ui->help->setProperty("menuButton", true);
    m_ui->help->setProperty("menuButtonTopBordered", true);

    m_ui->userName->setProperty("editableLabel", true);
    m_ui->getSubscriptionInfo->setProperty("isUpdateButton", true);
    m_ui->renewSubscription->setProperty("menuButton", true);
    m_ui->renewSubscription->setProperty("menuButtonTopBordered", true);
    m_ui->changePassword->setProperty("menuButton", true);
    m_ui->logout->setProperty("menuButton", true);

    const QString panelStyle = "background-color: palette(window);"
                               "border-top-width: 1px;"
                               "border-top-style: solid;"
                               "border-top-color: palette(dark);";
    m_ui->menuActions->setStyleSheet(QString("#menuActions { %1 }").arg(panelStyle));
    m_ui->menuAccount->setStyleSheet(QString("#menuAccount { %1 }").arg(panelStyle));
}
