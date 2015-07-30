#include "ScenarioReviewPanel.h"

#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/PopupWidget/PopupWidget.h>
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
	m_editor(_editor),
	m_activateButton(new FlatButton(this)),
	m_textColor(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-color.png"), this)),
	m_textBgColor(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-bg-color.png"), this)),
	m_textHighlight(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-bg-color.png"), this)),
	m_comment(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/comment.png"), this)),
	m_done(new FlatButton(this)),
	m_clear(new FlatButton(this))
{
	initView();
	initConnections();
	initStyleSheet();
}

void ScenarioReviewPanel::aboutUpdateActionsEnable()
{
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
						&& ((textBlock.position() + range.start + range.length) <= toCursorPos)) {
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
	m_textColor->setEnabled(hasSelectedText);
	//
	// ... выделение в зависимости от настроек
	//
	if (StorageFacade::settingsStorage()->value(
			"scenario-editor/review/use-highlight",
			SettingsStorage::ApplicationSettings).toInt() == false) {
		m_textBgColor->setEnabled(hasSelectedText);
		m_textBgColor->setVisible(true);
		m_textHighlight->setVisible(false);
	} else {
		m_textHighlight->setEnabled(hasSelectedText);
		m_textHighlight->setVisible(true);
		m_textBgColor->setVisible(false);
	}
	m_comment->setEnabled(hasSelectedText);
	m_done->setEnabled(showDone);
	m_done->setChecked(reviewDone);
	m_clear->setEnabled(hasReview);
}

void ScenarioReviewPanel::aboutChangeTextColor(const QColor& _color)
{
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
	BusinessLogic::ScenarioReviewModel* model = reviewModel();
	model->setReviewMarkIsDone(m_editor->textCursor().position(), _done);
}

void ScenarioReviewPanel::clearReview()
{
	BusinessLogic::ScenarioReviewModel* model = reviewModel();
	const QTextCursor cursor = m_editor->textCursor();
	const int from = qMin(cursor.selectionStart(), cursor.selectionEnd());
	const int to = qMax(cursor.selectionStart(), cursor.selectionEnd());
	model->removeMarks(from, to);
}

void ScenarioReviewPanel::initView()
{
	m_activateButton->setIcons(QIcon(":/Graphics/Icons/Editing/review.png"));
	m_activateButton->setToolTip(tr("Review"));
	m_activateButton->setCheckable(true);

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
	m_done->setIcons(QIcon(":/Graphics/Icons/Review/done.png"));
	m_done->setCheckable(true);
	m_done->setToolTip(tr("Mark as done"));
	m_clear->setIcons(QIcon(":/Graphics/Icons/Review/clear.png"));
	m_clear->setToolTip(tr("Clear"));

	PopupWidget* popup = new PopupWidget(this);
	popup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	popup->setAlignment(Qt::AlignTop | Qt::AlignRight);
	popup->setOrientation(Qt::Horizontal);
	connect(m_activateButton, SIGNAL(toggled(bool)), popup, SLOT(showPopup(bool)));

	QHBoxLayout* popupLayout = new QHBoxLayout;
	popupLayout->setContentsMargins(QMargins());
	popupLayout->setSpacing(0);
	popupLayout->addWidget(m_textColor);
	popupLayout->addWidget(m_textBgColor);
	popupLayout->addWidget(m_textHighlight);
	popupLayout->addWidget(m_comment);
	popupLayout->addWidget(m_done);
	popupLayout->addWidget(m_clear);
	popup->frame()->setLayout(popupLayout);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_activateButton);
	layout->addWidget(popup);
	setLayout(layout);

	aboutUpdateActionsEnable();

	QTimer::singleShot(0, popup, SLOT(hidePopup()));
}

void ScenarioReviewPanel::initConnections()
{
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateActionsEnable()));
	connect(m_editor, SIGNAL(selectionChanged()), this, SLOT(aboutUpdateActionsEnable()));

	connect(m_activateButton, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));

	connect(m_textColor, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextColor(QColor)));
	connect(m_textBgColor, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextBgColor(QColor)));
	connect(m_textHighlight, SIGNAL(clicked(QColor)), this, SLOT(aboutChangeTextHighlight(QColor)));
	connect(m_comment, SIGNAL(clicked(QColor)), this, SLOT(aboutAddComment(QColor)));
	connect(m_done, SIGNAL(toggled(bool)), this, SLOT(doneReview(bool)));
	connect(m_clear, SIGNAL(clicked()), this, SLOT(clearReview()));
}

void ScenarioReviewPanel::initStyleSheet()
{
	m_activateButton->setProperty("inTopPanel", true);
	m_textColor->setProperty("inTopPanel", true);
	m_textBgColor->setProperty("inTopPanel", true);
	m_textHighlight->setProperty("inTopPanel", true);
	m_comment->setProperty("inTopPanel", true);
	m_done->setProperty("inTopPanel", true);
	m_clear->setProperty("inTopPanel", true);

	m_textColor->setProperty("reviewPanel", true);
	m_textBgColor->setProperty("reviewPanel", true);
	m_textHighlight->setProperty("reviewPanel", true);
	m_comment->setProperty("reviewPanel", true);
}

BusinessLogic::ScenarioReviewModel* ScenarioReviewPanel::reviewModel() const
{
	BusinessLogic::ScenarioReviewModel* model = 0;
	if (ScenarioTextDocument* document = qobject_cast<ScenarioTextDocument*>(m_editor->document())) {
		model = qobject_cast<BusinessLogic::ScenarioReviewModel*>(document->reviewModel());
	}
	return model;
}

