#include "ScenarioReviewView.h"

#include "ScenarioReviewItemDelegate.h"
#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>
#include <3rd_party/Widgets/QWidgetListView/qwidgetlistview.h>

#include <QApplication>
#include <QDateTime>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QToolButton>
#include <QVBoxLayout>

using BusinessLogic::ScenarioTextDocument;
using BusinessLogic::ScenarioReviewModel;
using UserInterface::ReviewMarkWidget;
using UserInterface::ScenarioReviewItemDelegate;
using UserInterface::ScenarioReviewWidget;
using UserInterface::ScenarioReviewView;

namespace {

	/**
	 * @brief Размер иконок в кнопках
	 */
	const QSize ICON_SIZE(16, 16);

	/**
	 * @brief Размер кнопок
	 */
	const QSize BUTTON_SIZE(22, 22);

	/**
	 * @brief Ширина пенели с комментариями
	 */
	const int REVIEW_VIEW_MIN_WIDTH = 100;

	/**
	 * @brief Текст для заметки без комментария
	 */
	const QString EMPTY_REVIEW_TEXT = " ";


	/**
	 * @brief Ширина цветовой метки комментария
	 */
	const int COLOR_MARK_WIDTH = 12;
}

Q_DECLARE_METATYPE(QTextLayout::FormatRange)


ReviewMarkWidget::ReviewMarkWidget(QWidget* _parent) :
	QWidget(_parent),
	m_color(new QLabel(this)),
	m_author(new QLabel(this)),
	m_date(new QLabel(this)),
	m_comment(new QLabel(this)),
	m_isSelected(false)
{
	initView();
	initConnections();
}

void ReviewMarkWidget::setReviewMark(const QColor& _color, const QString& _author,
	const QString& _date, const QString& _comment, bool _done)
{
	if (_color.isValid()) {
		m_color->setStyleSheet(QString("background-color: %1; border: 1px solid palette(midlight); border-right: none;").arg(_color.name()));
	} else {
		m_color->setStyleSheet("");
	}
	//
	m_author->setText(_author);
	//
	m_date->setText(QDateTime::fromString(_date, Qt::ISODate).toString("dd.MM.yyyy hh:mm"));
	//
	if (_comment.isEmpty() || _done) {
		m_comment->hide();
	} else {
		m_comment->show();
		m_comment->setText(_comment);
	}

	repaint();
}

void ReviewMarkWidget::setSelected(bool _selected)
{
	if (m_isSelected != _selected) {
		m_isSelected = _selected;

		repaint();
	}
}

QString ReviewMarkWidget::comment() const
{
	return m_comment->text();
}

void ReviewMarkWidget::paintEvent(QPaintEvent* _event)
{
	//
	// Настроим стиль
	//

	//
	// Если это ответ на редакторскую заметку
	//
	if (m_color->styleSheet().isEmpty()) {
		setStyleSheet(
			"[reviewMark=true] { background-color: palette(window); border: 1px solid palette(midlight); }"
			"QLabel { color: palette(text); }");
	}
	//
	// Если это сама заметка
	//
	else {
		//
		// Выделен
		//
		if (m_isSelected) {
			setStyleSheet(
				"[reviewMark=true] { background-color: palette(highlight); border: 1px solid palette(midlight); }"
				"QLabel { color: palette(highlighted-text); }");
		}
		//
		// Не выделен
		//
		else {
			setStyleSheet(
				"[reviewMark=true] { background-color: palette(base); border: 1px solid palette(midlight); }"
				"QLabel { color: palette(text); }");
		}
	}

	m_date->setStyleSheet("color: palette(dark);");

	QWidget::paintEvent(_event);
}

void ReviewMarkWidget::initView()
{
	m_color->setFixedWidth(COLOR_MARK_WIDTH);
	QFont authorFont = m_author->font();
	authorFont.setBold(true);
	m_author->setFont(authorFont);
	QFont dateFont = m_date->font();
#ifdef Q_OS_WIN
	dateFont.setPointSize(dateFont.pointSize() - 1);
#else
	dateFont.setPointSize(dateFont.pointSize() - 4);
#endif
	dateFont.setBold(true);
	m_date->setFont(dateFont);
	m_comment->setWordWrap(true);

	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->setContentsMargins(5, 5, 5, 5);
	rightLayout->setSpacing(0);
	rightLayout->addWidget(m_author);
	rightLayout->addWidget(m_date);
	rightLayout->addSpacing(4);
	rightLayout->addWidget(m_comment);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->addWidget(m_color);
	layout->addLayout(rightLayout);

	QWidget* container = new QWidget(this);
	container->setProperty("reviewMark", true);
	container->setLayout(layout);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->setContentsMargins(QMargins());
	mainLayout->setSpacing(0);
	mainLayout->addWidget(container);
	setLayout(mainLayout);
}

void ReviewMarkWidget::initConnections()
{

}


// ********


ScenarioReviewWidget::ScenarioReviewWidget(QAbstractItemModel* _model, const QModelIndex& _index) :
	QtModelWidget(_model, _index),
	m_layout(new QVBoxLayout(this))
{
	initView();
	initConnections();
	initStylesheet();
}

void ScenarioReviewWidget::dataChanged()
{
	const bool done = model()->data(index(), ScenarioReviewModel::IsDoneRole).toBool();
	const QColor color = model()->data(index(), Qt::DecorationRole).value<QColor>();
	const QStringList authors = model()->data(index(), ScenarioReviewModel::CommentsAuthorsRole).toStringList();
	const QStringList dates = model()->data(index(), ScenarioReviewModel::CommentsDatesRole).toStringList();
	const QStringList comments = model()->data(index(), ScenarioReviewModel::CommentsRole).toStringList();
	int size = comments.size();
	if (size == 0) {
		size = 1;
	}

	//
	// Корректируем кол-во виджетов
	//
	while (m_reviewMarks.size() != size) {
		if (m_reviewMarks.size() > size) {
			ReviewMarkWidget* widget = m_reviewMarks.takeLast();
			m_layout->removeWidget(widget);
			widget->deleteLater();
		} else {
			ReviewMarkWidget* widget = new ReviewMarkWidget(this);
			m_reviewMarks.append(widget);
			m_layout->addWidget(widget);
		}
	}

	//
	// Наполняем информацией
	//
	for (int commentIndex = 0; commentIndex < m_reviewMarks.size(); ++commentIndex) {
		ReviewMarkWidget* widget = m_reviewMarks.at(commentIndex);
		widget->setReviewMark(commentIndex == 0 ? color : QColor(), authors.value(commentIndex),
			dates.value(commentIndex), comments.value(commentIndex), done);
		widget->setSelected(isSelected());
		if (commentIndex > 0) {
			widget->setVisible(!done);
		}
	}
}

void ScenarioReviewWidget::aboutContextMenuRequested(const QPoint& _pos)
{
	QMenu* menu = new QMenu(this);
	QAction* edit = menu->addAction(tr("Edit"));
	QAction* reply = menu->addAction(tr("Reply"));
	QAction* done = menu->addAction(tr("Done"));
	done->setCheckable(true);
	done->setChecked(model()->data(index(), ScenarioReviewModel::IsDoneRole).toBool());
	menu->addSeparator();
	QAction* remove = menu->addAction(tr("Remove"));

	//
	// Определим комментарий над коротым нажата кнопка мыши
	//
	int commentIndex = 0;
	QApplication::widgetAt(_pos);
	foreach (const QWidget* widget, m_reviewMarks) {
		QRect widgetRect = widget->rect();
		//
		// NOTE: Позиционирование по y почему-то увеличено в 2 раза
		//
		widgetRect.moveTop(widget->mapToParent(widget->pos()).y() / 2);
		if (widgetRect.contains(_pos)) {
			break;
		} else {
			++commentIndex;
		}
	}

	QAction* toggled = menu->exec(mapToGlobal(_pos));
	if (toggled != 0) {
		if (toggled == edit) {
			aboutEdit(commentIndex);
		} else if (toggled == reply) {
			aboutReply();
		} else if (toggled == done) {
			aboutDone(done->isChecked());
		} else if (toggled == remove) {
			aboutDelete(commentIndex);
		}
	}

	menu->deleteLater();
}

void ScenarioReviewWidget::aboutEdit(int _commentIndex)
{
	if (index().isValid()) {
		const QString comment = QLightBoxInputDialog::getText(parentWidget(), QString::null,
									tr("Comment"), m_reviewMarks.at(_commentIndex)->comment());
		if (!comment.isEmpty()) {
			ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
			reviewModel->updateReviewMarkComment(index(), _commentIndex, comment);
		}
	}
}

void ScenarioReviewWidget::aboutReply()
{
	if (index().isValid()) {
		const QString comment = QLightBoxInputDialog::getText(parentWidget(), QString::null, tr("Reply"));
		if (!comment.isEmpty()) {
			ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
			reviewModel->addReviewMarkComment(index(), comment);
		}
	}
}

void ScenarioReviewWidget::aboutDone(bool _done)
{
	if (index().isValid()) {
		ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
		reviewModel->setReviewMarkIsDone(index(), _done);
	}
}

void ScenarioReviewWidget::aboutDelete(int _commentIndex)
{
	if (index().isValid()) {
		ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
		reviewModel->removeMark(index(), _commentIndex);
	}
}

void ScenarioReviewWidget::initView()
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setMinimumWidth(REVIEW_VIEW_MIN_WIDTH);

	m_layout->setContentsMargins(QMargins());
	m_layout->setSpacing(0);
}

void ScenarioReviewWidget::initConnections()
{
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(aboutContextMenuRequested(QPoint)));
//	connect(m_edit, SIGNAL(clicked(bool)), this, SLOT(aboutEdit()));
//	connect(m_done, SIGNAL(clicked(bool)), this, SLOT(aboutDone()));
//	connect(m_delete, SIGNAL(clicked(bool)), this, SLOT(aboutDelete()));
}

void ScenarioReviewWidget::initStylesheet()
{
//	m_container->setProperty("reviewWidgetContainer", true);
//	m_container->setStyleSheet(
//		"QToolButton { background-color: palette(base); border: 1px solid transparent; min-width: 20px; padding: 3px; } "
//		"QToolButton:hover { border: 1px solid palette(midlight); }"
//		"QToolButton:pressed { background-color: palette(midlight); }"
//		"QToolButton:checked { background-color: palette(midlight); }"
//		"[reviewWidgetContainer=true] { border: none; border-bottom: 1px solid palette(text); }");
}


// ********


UserInterface::ScenarioReviewView::ScenarioReviewView(QWidget* _parent) :
	QWidget(_parent),
	m_view(new QListView(this)),
	m_editor(0)
{
	initView();
	initConnections();
	initStylesheet();
}

void UserInterface::ScenarioReviewView::setEditor(ScenarioTextEdit* _editor)
{
	if (m_editor != _editor) {
		m_editor = _editor;

		if (m_editor != 0) {
			aboutUpdateModel();
			connect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutUpdateModel()));
			connect(m_editor, SIGNAL(reviewChanged()), this, SLOT(aboutUpdateModel()));
			connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutSelectMark()));
		}
	}
}

void ScenarioReviewView::aboutUpdateModel()
{
	if (m_editor != 0) {
		if (ScenarioTextDocument* document = qobject_cast<ScenarioTextDocument*>(m_editor->document())) {
			ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(document->reviewModel());
			m_view->setModel(reviewModel);
		}
	}
}

void ScenarioReviewView::aboutMoveCursorToMark(const QModelIndex& _index)
{
	//
	// Отключаемся, чтобы не моргал выделенный элемент
	//
	disconnect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutSelectMark()));

	if (ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(m_view->model())) {
		const int cursorPosition = reviewModel->markStartPosition(_index);
		const int length = reviewModel->markLength(_index);
		QTextCursor cursor = m_editor->textCursor();
		cursor.setPosition(cursorPosition + length);
		m_editor->setTextCursorReimpl(cursor);
		m_editor->ensureCursorVisibleReimpl(true);
		m_editor->clearFocus();
		m_editor->setFocus();
	}

	//
	// Восстанавливаем соединение
	//
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutSelectMark()));
}

void ScenarioReviewView::aboutSelectMark()
{
	const int cursorPosition = m_editor->textCursor().position();
	if (ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(m_view->model())) {
		const QModelIndex index = reviewModel->indexForPosition(cursorPosition);
		m_view->clearSelection();
		m_view->setCurrentIndex(index);
		if (index.isValid()) {
			m_view->scrollTo(index);
		}
	}
}

void ScenarioReviewView::initView()
{
	m_view->setItemDelegate(new ScenarioReviewItemDelegate(m_view));
	m_view->setAlternatingRowColors(true);
	m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->addWidget(m_view);
	setLayout(layout);
}

void ScenarioReviewView::initConnections()
{
	connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(aboutMoveCursorToMark(QModelIndex)));
}

void ScenarioReviewView::initStylesheet()
{
	setStyleSheet("QListView { show-decoration-selected: 0; }");
}
