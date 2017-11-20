#include "ScriptZenModeControls.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QApplication>
#include <QCheckBox>
#include <QPainter>
#include <QPushButton>
#include <QShortcut>
#include <QTextBlock>
#include <QVBoxLayout>

using UserInterface::ScriptZenModeControls;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioTemplate;
using BusinessLogic::ScenarioTemplateFacade;

namespace {
    /**
     * @brief Свойство виджета, в котором сохраняется стиль блока
     */
    const char* STYLE_PROPERTY_KEY = "block_style";

    /**
     * @brief Создать кнопку применения стиля
     */
    QPushButton* createStyleButton(ScriptZenModeControls* _parent) {
        QPushButton* styleButton = new QPushButton(_parent);
        styleButton->setCheckable(true);
        styleButton->setProperty("leftAlignedText", true);

        _parent->connect(styleButton, &QPushButton::clicked, _parent, &ScriptZenModeControls::changeStyle);

        return styleButton;
    }
}


ScriptZenModeControls::ScriptZenModeControls(QWidget* _parent) :
    QFrame(_parent),
    m_quit(new FlatButton(this)),
    m_keyboardSound(new QCheckBox(this))
{
    setFrameShape(QFrame::NoFrame);

    qApp->installEventFilter(this);

    m_quit->setIconSize(QSize(36, 36));
    m_quit->setIcons(QIcon(":/Graphics/Icons/Editing/close.png"));
    m_quit->setShortcut(QKeySequence("F5"));
    connect(m_quit, &FlatButton::clicked, this, &ScriptZenModeControls::quitPressed);

    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    m_buttons << ::createStyleButton(this);
    reinitBlockStyles();

    m_keyboardSound->setText(tr("Typewriter sound"));

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addSpacing(10);
    layout->addWidget(m_quit, 0, Qt::AlignRight);
    layout->addStretch(1);
    foreach (QPushButton* button, m_buttons) {
        layout->addWidget(button);
    }
    layout->addSpacing(20);
    layout->addWidget(m_keyboardSound);
    layout->addStretch(2);

    setLayout(layout);

    m_hideTimer.setSingleShot(true);
    m_hideTimer.setInterval(1000);
    connect(&m_hideTimer, &QTimer::timeout, this, &ScriptZenModeControls::hide);
    m_hideTimer.start();
}

void ScriptZenModeControls::setEditor(UserInterface::ScenarioTextEdit* _editor)
{
    if (m_editor != _editor) {
        m_editor = _editor;

        if (m_editor != 0) {
            connect(m_keyboardSound, &QCheckBox::toggled, m_editor, &ScenarioTextEdit::setKeyboardSoundEnabled);
            connect(m_editor, &ScenarioTextEdit::currentStyleChanged, this, &ScriptZenModeControls::updateStyleButtons);
        }
    }
}

void ScriptZenModeControls::reinitBlockStyles()
{
    ScenarioTemplate style = ScenarioTemplateFacade::getTemplate();
    const bool BEAUTIFY_NAME = true;

    //
    // Настраиваем в зависимости от доступности стиля
    //
    int itemIndex = 0;

    QList<ScenarioBlockStyle::Type> types;
    types << ScenarioBlockStyle::SceneHeading
          << ScenarioBlockStyle::SceneCharacters
          << ScenarioBlockStyle::Action
          << ScenarioBlockStyle::Character
          << ScenarioBlockStyle::Dialogue
          << ScenarioBlockStyle::Parenthetical
          << ScenarioBlockStyle::Title
          << ScenarioBlockStyle::Note
          << ScenarioBlockStyle::Transition
          << ScenarioBlockStyle::NoprintableText
          << ScenarioBlockStyle::Lyrics;

    foreach (ScenarioBlockStyle::Type type, types) {
        if (style.blockStyle(type).isActive()) {
            m_buttons.at(itemIndex)->setVisible(true);
            m_buttons.at(itemIndex)->setText(ScenarioBlockStyle::typeName(type, BEAUTIFY_NAME));
            m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, type);
            ++itemIndex;
        }
    }

    for (; itemIndex < m_buttons.count(); ++itemIndex) {
        m_buttons.at(itemIndex)->setVisible(false);
    }
}

void ScriptZenModeControls::changeStyle()
{
    if (QPushButton* button = qobject_cast<QPushButton*>(sender())) {
        button->setChecked(true);
        ScenarioBlockStyle::Type type =
                (ScenarioBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt();
        if (m_editor != 0) {
            m_editor->changeScenarioBlockType(type);
        }
    }
}

void ScriptZenModeControls::updateStyleButtons()
{
    ScenarioBlockStyle::Type currentType = m_editor->scenarioBlockType();
    foreach (QPushButton* button, m_buttons) {
        button->setChecked(
            (ScenarioBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt() == currentType);
    }
}

void ScriptZenModeControls::activate(bool _active)
{
    if (m_active != _active) {
        m_active = _active;

        if (m_active) {
            raise();
            show();
            m_editor->setKeyboardSoundEnabled(m_keyboardSound->isChecked());
        } else {
            hide();
        }
    }
}

bool ScriptZenModeControls::eventFilter(QObject* _watched, QEvent* _event)
{
    if (m_active) {
        if (_watched == parent()
            && _event->type() == QEvent::Resize) {
            resize(sizeHint().width(), parentWidget()->height());
            move(parentWidget()->width() - width() - 20, 0);
        } else if (_event->type() == QEvent::MouseMove
                   || _event->type() == QEvent::Wheel
                   || _event->type() == QEvent::MouseButtonPress
                   || _event->type() == QEvent::MouseButtonRelease) {
            show();
            m_hideTimer.start();
        }
    }

    return QFrame::eventFilter(_watched, _event);
}
