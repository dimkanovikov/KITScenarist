#include "ScriptZenModeControls.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPropertyAnimation>
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
        styleButton->setFocusPolicy(Qt::NoFocus);

        _parent->connect(styleButton, &QPushButton::clicked, _parent, &ScriptZenModeControls::changeStyle);

        return styleButton;
    }
}


ScriptZenModeControls::ScriptZenModeControls(QWidget* _parent) :
    QFrame(_parent),
    m_quit(new FlatButton(this)),
    m_keyboardSound(new QCheckBox(this)),
    m_opacityEffect(new QGraphicsOpacityEffect(this))
{
    setFrameShape(QFrame::NoFrame);

    qApp->installEventFilter(this);

    setGraphicsEffect(m_opacityEffect);

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
    m_hideTimer.setInterval(2000);
    connect(&m_hideTimer, &QTimer::timeout, this, &ScriptZenModeControls::hideAnimated);
    m_hideTimer.start();
}

void ScriptZenModeControls::setEditor(UserInterface::ScenarioTextEdit* _editor)
{
    if (m_editor != _editor) {
        m_editor = _editor;

        if (m_editor != 0) {
            connect(m_keyboardSound, &QCheckBox::toggled, [this] (bool _checked) {
                if (m_active) {
                    m_editor->setKeyboardSoundEnabled(_checked);
                }
            });
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
            m_editor->changeScenarioBlockTypeForSelection(type);
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
            showAnimated();
            m_editor->setKeyboardSoundEnabled(m_keyboardSound->isChecked());
            m_hideTimer.start();
        } else {
            hideAnimated();
        }
    }
}

bool ScriptZenModeControls::eventFilter(QObject* _watched, QEvent* _event)
{
    if (m_active) {
        if (_watched == parent()
            && _event->type() == QEvent::Resize) {
            resize(sizeHint().width(), parentWidget()->height());
            const int margin = 20;
            const int xCoordinate = QLocale().textDirection() == Qt::LeftToRight
                                    ? parentWidget()->width() - width() - margin
                                    : margin;
            move(xCoordinate, 0);
        } else if (_event->type() == QEvent::MouseMove
                   || _event->type() == QEvent::Wheel
                   || _event->type() == QEvent::MouseButtonPress
                   || _event->type() == QEvent::MouseButtonRelease) {
            //
            // Показываем панель не после первого движения, а лишь после определённых усилий пользователя
            // Делается это для того, чтобы случайные прикосновения к мыши и тачпаду не приводили
            // к отображению вспомогательной панели, т.к. это отвлекает
            //
            static int s_eventsCounter = 0;
            const int maxEvents = 300;
            static qint64 s_lastActivateTime = QDateTime::currentMSecsSinceEpoch();
            const qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
            const qint64 showTimeout = 2000;
            //
            // Если со времени последней активности прошло больше showTimeout, то сбрасываем счётчик
            // событий и назначаем временем последней активности - текущее
            //
            if (currentTime - s_lastActivateTime > showTimeout) {
                s_lastActivateTime = currentTime;
                s_eventsCounter = 0;
            }
            //
            // Если пользователь совершил достаточно действий, отобразим себя
            //
            if (s_eventsCounter == maxEvents) {
                s_eventsCounter = 0;
                showAnimated();
                m_hideTimer.start();
            } else {
                ++s_eventsCounter;
            }
        }
    }

    return QFrame::eventFilter(_watched, _event);
}

void ScriptZenModeControls::showAnimated()
{
    if (isVisible()) {
        return;
    }

    m_opacityEffect->setOpacity(0);
    show();

    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    opacityAnimation->setDuration(180);
    opacityAnimation->setEasingCurve(QEasingCurve::OutCirc);
    opacityAnimation->setStartValue(0);
    opacityAnimation->setEndValue(1);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void ScriptZenModeControls::hideAnimated()
{
    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    opacityAnimation->setDuration(180);
    opacityAnimation->setEasingCurve(QEasingCurve::InCirc);
    opacityAnimation->setStartValue(1);
    opacityAnimation->setEndValue(0);
    connect(opacityAnimation, &QPropertyAnimation::finished, this, &ScriptZenModeControls::hide);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
