#include "MenuView.h"
#include "ui_MenuView.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QMenu>

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
}

using UserInterface::MenuView;


MenuView::MenuView(QWidget* _parent) :
    QWidget(_parent),
    m_ui(new Ui::MenuView)
{
    m_ui->setupUi(this);

    initView();
    initMenuButtons();
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
            QPushButton* menuButton = m_menuButtons[menuButtonIndex];
            if (menuButton != nullptr) {
                connect(menuButton, &QPushButton::clicked, this, &MenuView::hideRequested);
                connect(menuButton, &QPushButton::clicked, m_menu->actions()[menuButtonIndex], &QAction::trigger);
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

bool MenuView::event(QEvent* _event)
{
    if (_event->type() == QEvent::PaletteChange) {
        initMenuButtons();
    }

    return QWidget::event(_event);
}

void MenuView::initView()
{
    m_menuButtons << m_ui->createProject
                  << m_ui->openProject
                  << m_ui->saveProject
                  << m_ui->saveProjectAs
                  << nullptr
                  << m_ui->importProject
                  << m_ui->exportProject
                  << m_ui->printPreview;

    m_ui->version->setText(QApplication::applicationVersion());
}

void MenuView::initMenuButtons()
{
    auto updateButton = [this] (QPushButton* _button) {
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
}

void MenuView::initStyleSheet()
{
    m_ui->account->setProperty("menuAccount", true);
    m_ui->login->setProperty("link", true);

    m_ui->createProject->setProperty("menuButton", true);
    m_ui->createProject->setProperty("menuButtonTopBordered", true);
    m_ui->openProject->setProperty("menuButton", true);
    m_ui->saveProject->setProperty("menuButton", true);
    m_ui->saveProjectAs->setProperty("menuButton", true);
    m_ui->importProject->setProperty("menuButton", true);
    m_ui->importProject->setProperty("menuButtonTopBordered", true);
    m_ui->exportProject->setProperty("menuButton", true);
    m_ui->printPreview->setProperty("menuButton", true);
    m_ui->help->setProperty("menuButton", true);
    m_ui->help->setProperty("menuButtonTopBordered", true);
}
