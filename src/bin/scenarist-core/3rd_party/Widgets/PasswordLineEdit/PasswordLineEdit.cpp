#include "PasswordLineEdit.h"

#include <QToolButton>
#include <QStyle>

PasswordLineEdit::PasswordLineEdit(QWidget* _parent) :
    QLineEdit(_parent),
    m_eye(new QToolButton(this))
{
    m_eye->setIcon(QIcon(":Graphics/Icons/eye.png"));
    m_eye->setIconSize(QSize(15, 15));
    m_eye->setCursor(Qt::ArrowCursor);
    m_eye->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    m_eye->setVisible(true);
    connect(m_eye, SIGNAL(clicked()), this, SLOT(eyeClicked()));

    isAsterisk = true;
    QLineEdit::setEchoMode(QLineEdit::Password);
}

void PasswordLineEdit::resizeEvent(QResizeEvent *_event)
{
    QLineEdit::resizeEvent(_event);

    //
    // Корректируем положение кнопки
    //
    const QSize eyeSizeHint = m_eye->sizeHint();
    const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    m_eye->move(
        rect().right() - frameWidth - eyeSizeHint.width(),
        (rect().bottom() + 3 - eyeSizeHint.height()) / 2
        );
}

void PasswordLineEdit::eyeClicked()
{
    if(isAsterisk) {
        m_eye->setIcon(QIcon(":Graphics/Icons/eye-off.png"));
        QLineEdit::setEchoMode(QLineEdit::Normal);
    }
    else {
        m_eye->setIcon(QIcon(":Graphics/Icons/eye.png"));
        QLineEdit::setEchoMode(QLineEdit::Password);
    }
    isAsterisk = !isAsterisk;
}

void PasswordLineEdit::resetAsterisk()
{
    isAsterisk = false;
    eyeClicked();
}
