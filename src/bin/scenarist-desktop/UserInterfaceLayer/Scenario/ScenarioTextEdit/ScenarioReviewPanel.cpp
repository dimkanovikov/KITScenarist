#include "ScenarioReviewPanel.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>
#include <3rd_party/Widgets/WAF/Animation.h>

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

	/**
	 * @brief Сохранить используемый цвет
	 */
	static void saveColor(const QString& _key, const QColor& _color) {
		StorageFacade::settingsStorage()->setValue(
			_key, _color.name(), SettingsStorage::ApplicationSettings);
	}

	/**
	 * @brief Сохранить используемый цвет текста
	 */
	static void saveTextColor(const QColor& _color) {
		saveColor("scenario-editor/review/text-color", _color);
	}

	/**
	 * @brief Сохранить используемый цвет фона текста
	 */
	static void saveHighlightColor(const QColor& _color) {
		saveColor("scenario-editor/review/highlight-color", _color);
	}

	/**
	 * @brief Сохранить используемый цвет комментария
	 */
	static void saveCommentColor(const QColor& _color) {
		saveColor("scenario-editor/review/comment-color", _color);
	}

	/**
	 * @brief Загрузить используемый цвет
	 */
	static QColor loadColor(const QString& _key) {
		return QColor(StorageFacade::settingsStorage()->value(
						  _key, SettingsStorage::ApplicationSettings));
	}

	/**
	 * @brief Загрузить используемый цвет текста
	 */
	static QColor loadTextColor() {
		return loadColor("scenario-editor/review/text-color");
	}

	/**
	 * @brief Загрузить используемый цвет выделения
	 */
	static QColor loadHighlightColor() {
		return loadColor("scenario-editor/review/highlight-color");
	}

	/**
	 * @brief Загрузить используемый цвет комментария
	 */
	static QColor loadCommentColor() {
		return loadColor("scenario-editor/review/comment-color");
	}
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
					reviewDone = range.format.boolProperty(ScenarioBlockStyle::PropertyIsDone);
					break;
				}
			}
		}
	}

	//
	// Настроим кнопки
	//
	// ... выделение в зависимости от настроек
	//
	if (StorageFacade::settingsStorage()->value(
			"scenario-editor/review/use-highlight",
			SettingsStorage::ApplicationSettings).toInt() == false) {
		m_textBgColor->setVisible(true);
		m_textHighlight->setVisible(false);
	} else {
		m_textHighlight->setVisible(true);
		m_textBgColor->setVisible(false);
	}
	m_done->setChecked(reviewDone);
}

void ScenarioReviewPanel::aboutChangeTextColor(const QColor& _color)
{
	::saveTextColor(_color);

	if (m_editor->textCursor().hasSelection()) {
		BusinessLogic::ScenarioReviewModel* model = reviewModel();
		const int selectionStart = m_editor->textCursor().selectionStart();
		const int selectionEnd = m_editor->textCursor().selectionEnd();
		const int startPosition = qMin(selectionStart, selectionEnd);
		const int length = qMax(selectionStart, selectionEnd) - startPosition;
		model->setReviewMarkTextColor(startPosition, length, _color);
	}
}

void ScenarioReviewPanel::aboutChangeTextBgColor(const QColor& _color)
{
	::saveHighlightColor(_color);

	if (m_editor->textCursor().hasSelection()) {
		BusinessLogic::ScenarioReviewModel* model = reviewModel();
		const int selectionStart = m_editor->textCursor().selectionStart();
		const int selectionEnd = m_editor->textCursor().selectionEnd();
		const int startPosition = qMin(selectionStart, selectionEnd);
		const int length = qMax(selectionStart, selectionEnd) - startPosition;
		model->setReviewMarkTextBgColor(startPosition, length, _color);
	}
}

void ScenarioReviewPanel::aboutChangeTextHighlight(const QColor& _color)
{
	::saveHighlightColor(_color);

	if (m_editor->textCursor().hasSelection()) {
		BusinessLogic::ScenarioReviewModel* model = reviewModel();
		const int selectionStart = m_editor->textCursor().selectionStart();
		const int selectionEnd = m_editor->textCursor().selectionEnd();
		const int startPosition = qMin(selectionStart, selectionEnd);
		const int length = qMax(selectionStart, selectionEnd) - startPosition;
		model->setReviewMarkTextHighlight(startPosition, length, _color);
	}
}

void ScenarioReviewPanel::aboutAddComment(const QColor& _color)
{
	::saveCommentColor(_color);

	if (m_editor->textCursor().hasSelection()) {
		const QString comment = QLightBoxInputDialog::getLongText(parentWidget(), QString::null, tr("Comment"));
		if (!comment.isEmpty()) {
			BusinessLogic::ScenarioReviewModel* model = reviewModel();
			const int selectionStart = m_editor->textCursor().selectionStart();
			const int selectionEnd = m_editor->textCursor().selectionEnd();
			const int startPosition = qMin(selectionStart, selectionEnd);
			const int length = qMax(selectionStart, selectionEnd) - startPosition;
			model->setReviewMarkTextBgColor(startPosition, length, _color);
			model->setReviewMarkComment(startPosition, length, comment);
		}
	}
}

void ScenarioReviewPanel::doneReview(bool _done)
{
	if (BusinessLogic::ScenarioReviewModel* model = reviewModel()) {
		model->setReviewMarkIsDone(m_editor->textCursor().position(), _done);
	}
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
	m_textColor->setColor(::loadTextColor());
	m_textBgColor->setIconSize(ICON_SIZE);
	m_textBgColor->setColorsPane(ColoredToolButton::Google);
	m_textBgColor->setToolTip(tr("Change text background"));
	m_textBgColor->setColor(::loadHighlightColor());
	m_textHighlight->setIconSize(ICON_SIZE);
	m_textHighlight->setColorsPane(ColoredToolButton::WordHighlight);
	m_textHighlight->setToolTip(tr("Highlight text"));
	m_textHighlight->setColor(::loadHighlightColor());
	m_comment->setIconSize(ICON_SIZE);
	m_comment->setColorsPane(ColoredToolButton::Google);
	m_comment->setToolTip(tr("Add comment"));
	m_comment->setColor(::loadCommentColor());
	m_done->setIcons(QIcon(":/Graphics/Icons/Review/done.png"));
	m_done->setCheckable(true);
	m_done->setToolTip(tr("Mark as done"));
	m_clear->setIcons(QIcon(":/Graphics/Icons/Review/clear.png"));
	m_clear->setToolTip(tr("Clear"));

	QFrame* popup = new QFrame(this);
	popup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(m_activateButton, &FlatButton::toggled, [=](bool _show){
		const bool NOT_FIX_BACKGROUND = false;
		WAF::Animation::slide(popup, WAF::FromLeftToRight, NOT_FIX_BACKGROUND, _show);
	});

	QHBoxLayout* popupLayout = new QHBoxLayout;
	popupLayout->setContentsMargins(QMargins());
	popupLayout->setSpacing(0);
	popupLayout->addWidget(m_textColor);
	popupLayout->addWidget(m_textBgColor);
	popupLayout->addWidget(m_textHighlight);
	popupLayout->addWidget(m_comment);
	popupLayout->addWidget(m_done);
	popupLayout->addWidget(m_clear);
	popup->setLayout(popupLayout);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_activateButton);
	layout->addWidget(popup);
	setLayout(layout);

	aboutUpdateActionsEnable();

	popup->setMaximumWidth(0);
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

