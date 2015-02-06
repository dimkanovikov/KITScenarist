#include "AcceptebleLineEdit.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QStyle>
#include <QToolButton>


AcceptebleLineEdit::AcceptebleLineEdit(QWidget* _parent) :
	QLineEdit(_parent),
	m_accept(new QToolButton(this)),
	m_reject(new QToolButton(this))
{
	m_accept->setIcon(QIcon(":/Graphics/Icons/Editing/accept_green.png"));
	m_accept->setIconSize(QSize(12,12));
	m_accept->setCursor(Qt::ArrowCursor);
	m_accept->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	m_accept->hide();
	connect(m_accept, SIGNAL(clicked()), this, SLOT(acceptText()));

	m_reject->setIcon(QIcon(":/Graphics/Icons/Editing/red_cross.png"));
	m_reject->setIconSize(QSize(12,12));
	m_reject->setCursor(Qt::ArrowCursor);
	m_reject->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	m_reject->hide();
	connect(m_reject, SIGNAL(clicked()), this, SLOT(rejectText()));

	//
	// По умолчанию в вопросу будет говориться, что текст изменён
	//
	m_questionPrefix = tr("Text");

	//
	// При смене текста обновляем видимость элементов
	//
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(updateButtonsVisibility()));
}

void AcceptebleLineEdit::setAcceptedText(const QString& _text)
{
	m_acceptedText = _text;
	setText(_text);
}

void AcceptebleLineEdit::setQuestionPrefix(const QString& _prefix)
{
	if (m_questionPrefix != _prefix) {
		m_questionPrefix = _prefix;
	}
}

void AcceptebleLineEdit::resizeEvent(QResizeEvent* _event)
{
	QLineEdit::resizeEvent(_event);

	//
	// Корректируем положение кнопок
	//
	const QSize acceptSizeHint = m_accept->sizeHint();
	const QSize rejectSizeHint = m_reject->sizeHint();
	const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	const int buttonsSpacing = 4;

	m_accept->move(
		rect().right() - frameWidth - acceptSizeHint.width() - rejectSizeHint.width() - buttonsSpacing,
		(rect().bottom() + 3 - acceptSizeHint.height()) / 2
		);
	m_reject->move(
		rect().right() - frameWidth - rejectSizeHint.width(),
		(rect().bottom() + 3 - acceptSizeHint.height()) / 2
				);
}

void AcceptebleLineEdit::keyPressEvent(QKeyEvent* _event)
{
	switch ((Qt::Key)_event->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return: {
			acceptText();
			break;
		}

		case Qt::Key_Escape: {
			rejectText();
			break;
		}

		default: {
			QLineEdit::keyPressEvent(_event);
		}
	}
}

void AcceptebleLineEdit::focusOutEvent(QFocusEvent* _event)
{
	if (m_acceptedText != text()) {
		const QString question = tr("was changed from \"%1\" to \"%2\". Apply changes?").arg(m_acceptedText, text());
		if (QMessageBox::question(this, tr("Apply changes"),
				QString("%1 %2").arg(m_questionPrefix).arg(question))
			== QMessageBox::Yes) {
			acceptText();
		} else {
			rejectText();
		}
	}

	QLineEdit::focusOutEvent(_event);
}

void AcceptebleLineEdit::acceptText()
{
	if (m_acceptedText != text()) {
		const QString oldText = m_acceptedText;
		m_acceptedText = text();
		updateButtonsVisibility();

		emit textAccepted(m_acceptedText, oldText);
	}
}

void AcceptebleLineEdit::rejectText()
{
	if (m_acceptedText != text()) {
		setText(m_acceptedText);
		updateButtonsVisibility();

		emit textRejected();
	}
}

void AcceptebleLineEdit::updateButtonsVisibility()
{
	bool visible = m_acceptedText != text();
	m_accept->setVisible(visible);
	m_reject->setVisible(visible);
}

