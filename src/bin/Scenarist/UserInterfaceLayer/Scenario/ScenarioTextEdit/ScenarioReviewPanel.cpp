#include "ScenarioReviewPanel.h"

#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>

#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QTextBlock>
#include <QTextEdit>
#include <QTimer>

using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioTextDocument;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;
using UserInterface::ScenarioReviewPanel;
using UserInterface::ScenarioTextEdit;

namespace {
	/**
	 * @brief Размер иконок в кнопках
	 */
	const QSize ICON_SIZE(20, 20);
}


ScenarioReviewPanel::ScenarioReviewPanel(ScenarioTextEdit* _editor, QWidget* _parent) :
	QFrame(_parent),
	m_isActive(false),
	m_editor(_editor),
	m_textColor(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-color.png"), this)),
	m_textBgColor(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-bg-color.png"), this)),
	m_textHighlight(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-bg-color.png"), this)),
	m_comment(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/comment.png"), this)),
	m_done(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/done.png"), this)),
	m_clear(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/clear.png"), this))
{
	initView();
	initStyleSheet();
}

void ScenarioReviewPanel::setIsActive(bool _active)
{
	if (m_isActive != _active) {
		m_isActive = _active;

		if (_active) {
			initConnections();
			aboutShow();
		} else {
			removeConnections();
		}
	}
}

bool ScenarioReviewPanel::event(QEvent* _event)
{
	if (_event->type() == QEvent::KeyPress) {

		if (QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(_event)) {
			if (keyEvent->key() == Qt::Key_Escape) {
				close();
				return true;
			}
		}

		//
		// Проксируем событие в текстовый редактор,
		// и, если он его обработал, ничего не делаем
		//
		(static_cast<QObject *>(m_editor))->event(_event);
		if (_event->isAccepted()) {
			return true;
		}
	}

	return QFrame::event(_event);
}

void ScenarioReviewPanel::aboutShow()
{
	//
	// Если пользователь отпустил кнопку мыши, выделив необходимый текст, покажем панель,
	// а если он ещё не всё выделил, то попробуем это сделать немного попозже
	//

	if (QApplication::mouseButtons() == Qt::NoButton) {
		const bool hasSelectedText = m_editor->textCursor().hasSelection();
		bool hasReview = false;
		bool showDone = false;
		bool reviewDone = false;

		//
		// Для случая, когда выделения нет, курсор может стоять над выделением
		//
		if (!m_editor->textCursor().hasSelection()) {
			const QTextBlock textBlock = m_editor->textCursor().block();
			if (!textBlock.textFormats().isEmpty()) {
				const int cursorPosition = m_editor->textCursor().positionInBlock();
				foreach (const QTextLayout::FormatRange& range, textBlock.textFormats()) {
					if (range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark)
						&& range.start <= cursorPosition
						&& (range.start + range.length) > cursorPosition) {
						hasReview = true;
						showDone = true;
						reviewDone = range.format.boolProperty(ScenarioBlockStyle::PropertyIsDone);
						break;
					}
				}
			}
		}
		//
		// Если выделен некоторый текст
		//
		else {
			QTextCursor cursor = m_editor->textCursor();
			const int fromCursorPos = qMin(cursor.selectionStart(), cursor.selectionEnd());
			const int toCursorPos = qMax(cursor.selectionStart(), cursor.selectionEnd());
			cursor.setPosition(fromCursorPos);
			while (!cursor.atEnd()
				   && cursor.position() < toCursorPos) {
				const QTextBlock textBlock = cursor.block();
				if (!textBlock.textFormats().isEmpty()) {
					foreach (const QTextLayout::FormatRange& range, textBlock.textFormats()) {
						if (range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark)
							&& ((textBlock.position() + range.start) >= fromCursorPos)
							&& ((textBlock.position() + range.start + range.length) < toCursorPos)) {
							hasReview = true;
							break;
						}
					}
				}
				//
				// ... прерываем поиск, если нашли заметку
				//
				if (hasReview) {
					break;
				}

				cursor.movePosition(QTextCursor::EndOfBlock);
				cursor.movePosition(QTextCursor::NextBlock);
			}
		}

		//
		// Настроим кнопки
		//
		m_textColor->setVisible(hasSelectedText);
		//
		// ... выделение в зависимости от настроек
		//
		if (StorageFacade::settingsStorage()->value(
				"scenario-editor/review/use-highlight",
				SettingsStorage::ApplicationSettings).toInt() == false) {
			m_textBgColor->setVisible(hasSelectedText);
			m_textHighlight->setVisible(false);
		} else {
			m_textHighlight->setVisible(hasSelectedText);
			m_textBgColor->setVisible(false);
		}
		m_comment->setVisible(hasSelectedText);
		m_done->setVisible(showDone);
		m_done->setChecked(reviewDone);
		m_clear->setVisible(hasReview);

		//
		// Покажем панель, если нужно
		//
		if (hasSelectedText || hasReview) {
			QApplication::processEvents();

			QRect rect = m_editor->cursorRect();
			rect.moveTo(m_editor->mapToGlobal(m_editor->viewport()->mapToParent(rect.topLeft())));
			rect.moveTop(rect.top() - height());
			move(rect.topLeft());
			show();
		} else {
			close();
		}
	} else {
		QTimer::singleShot(100, this, SLOT(aboutShow()));
	}
}

void ScenarioReviewPanel::aboutChangeTextColor(const QColor& _color)
{
	close();

	BusinessLogic::ScenarioReviewModel* model = reviewModel();
	const int selectionStart = m_editor->textCursor().selectionStart();
	const int selectionEnd = m_editor->textCursor().selectionEnd();
	const int startPosition = qMin(selectionStart, selectionEnd);
	const int length = qMax(selectionStart, selectionEnd) - startPosition;
	model->setReviewMarkTextColor(startPosition, length, _color);

	m_editor->moveCursor(QTextCursor::Right);
}

void ScenarioReviewPanel::aboutChangeTextBgColor(const QColor& _color)
{
	close();

	BusinessLogic::ScenarioReviewModel* model = reviewModel();
	const int selectionStart = m_editor->textCursor().selectionStart();
	const int selectionEnd = m_editor->textCursor().selectionEnd();
	const int startPosition = qMin(selectionStart, selectionEnd);
	const int length = qMax(selectionStart, selectionEnd) - startPosition;
	model->setReviewMarkTextBgColor(startPosition, length, _color);

	m_editor->moveCursor(QTextCursor::Right);
}

void ScenarioReviewPanel::aboutChangeTextHighlight(const QColor& _color)
{
	close();

	BusinessLogic::ScenarioReviewModel* model = reviewModel();
	const int selectionStart = m_editor->textCursor().selectionStart();
	const int selectionEnd = m_editor->textCursor().selectionEnd();
	const int startPosition = qMin(selectionStart, selectionEnd);
	const int length = qMax(selectionStart, selectionEnd) - startPosition;
	model->setReviewMarkTextHighlight(startPosition, length, _color);

	m_editor->moveCursor(QTextCursor::Right);
}

void ScenarioReviewPanel::aboutAddComment(const QColor& _color)
{
	close();

	const QString comment = QLightBoxInputDialog::getText(parentWidget(), QString::null, tr("Comment"));
	if (!comment.isEmpty()) {
		BusinessLogic::ScenarioReviewModel* model = reviewModel();
		const int selectionStart = m_editor->textCursor().selectionStart();
		const int selectionEnd = m_editor->textCursor().selectionEnd();
		const int startPosition = qMin(selectionStart, selectionEnd);
		const int length = qMax(selectionStart, selectionEnd) - startPosition;
		model->setReviewMarkTextBgColor(startPosition, length, _color);
		model->setReviewMarkComment(startPosition, length, comment);

		m_editor->moveCursor(QTextCursor::Right);
	}
}

void ScenarioReviewPanel::doneReview(bool _done)
{
	close();

	BusinessLogic::ScenarioReviewModel* model = reviewModel();
	model->setReviewMarkIsDone(m_editor->textCursor().position(), _done);
}

void ScenarioReviewPanel::clearReview()
{
	close();

	BusinessLogic::ScenarioReviewModel* model = reviewModel();
	const QTextCursor cursor = m_editor->textCursor();
	const int from = qMin(cursor.selectionStart(), cursor.selectionEnd());
	const int to = qMax(cursor.selectionStart(), cursor.selectionEnd());
	model->removeMarks(from, to);
}

void ScenarioReviewPanel::initView()
{
	setWindowFlags(Qt::Popup);
	setFocusPolicy(Qt::NoFocus);
	setFocusProxy(m_editor);

	m_textColor->setIconSize(ICON_SIZE);
	m_textColor->setColorsPane(ColoredToolButton::Google);
	m_textColor->setToolTip(tr("Change text color"));
	m_textBgColor->setIconSize(ICON_SIZE);
	m_textBgColor->setColorsPane(ColoredToolButton::Google);
	m_textBgColor->setToolTip(tr("Change text background"));
	m_textHighlight->setIconSize(ICON_SIZE);
	m_textHighlight->setColorsPane(ColoredToolButton::WordHighlight);
	m_textHighlight->setToolTip(tr("Highlight text"));
	m_comment->setIconSize(ICON_SIZE);
	m_comment->setColorsPane(ColoredToolButton::Google);
	m_comment->setToolTip(tr("Add comment"));
	m_done->setIconSize(ICON_SIZE);
	m_done->setCheckable(true);
	m_done->setToolTip(tr("Mark as done"));
	m_clear->setIconSize(ICON_SIZE);
	m_clear->setToolTip(tr("Clear"));

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->addWidget(m_textColor);
	layout->addWidget(m_textBgColor);
	layout->addWidget(m_textHighlight);
	layout->addWidget(m_comment);
	layout->addWidget(m_done);
	layout->addWidget(m_clear);
	setLayout(layout);
}

void ScenarioReviewPanel::initConnections()
{
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutShow()));
	connect(m_editor, SIGNAL(selectionChanged()), this, SLOT(aboutShow()));

	connect(m_textColor, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextColor(QColor)));
	connect(m_textBgColor, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextBgColor(QColor)));
	connect(m_textHighlight, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextHighlight(QColor)));
	connect(m_comment, SIGNAL(clicked(QColor)), this, SLOT(aboutAddComment(QColor)));
	connect(m_done, SIGNAL(toggled(bool)), this, SLOT(doneReview(bool)));
	connect(m_clear, SIGNAL(clicked()), this, SLOT(clearReview()));
}

void ScenarioReviewPanel::removeConnections()
{
	disconnect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutShow()));
	disconnect(m_editor, SIGNAL(selectionChanged()), this, SLOT(aboutShow()));

	disconnect(m_textColor, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextColor(QColor)));
	disconnect(m_textBgColor, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextBgColor(QColor)));
	disconnect(m_comment, SIGNAL(clicked(QColor)), this, SLOT(aboutAddComment(QColor)));
	disconnect(m_done, SIGNAL(toggled(bool)), this, SLOT(doneReview(bool)));
	disconnect(m_clear, SIGNAL(clicked()), this, SLOT(clearReview()));
}

void ScenarioReviewPanel::initStyleSheet()
{

}

BusinessLogic::ScenarioReviewModel* ScenarioReviewPanel::reviewModel() const
{
	BusinessLogic::ScenarioReviewModel* model = 0;
	if (ScenarioTextDocument* document = qobject_cast<ScenarioTextDocument*>(m_editor->document())) {
		model = qobject_cast<BusinessLogic::ScenarioReviewModel*>(document->reviewModel());
	}
	return model;
}

