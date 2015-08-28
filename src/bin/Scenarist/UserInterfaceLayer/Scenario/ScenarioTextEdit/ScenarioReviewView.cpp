#include "ScenarioReviewView.h"

#include "ScenarioReviewItemDelegate.h"
#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>

#include <QKeyEvent>
#include <QMenu>

using BusinessLogic::ScenarioTextDocument;
using BusinessLogic::ScenarioReviewModel;
using UserInterface::ScenarioReviewView;
using UserInterface::ScenarioReviewItemDelegate;

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


ScenarioReviewView::ScenarioReviewView(QWidget* _parent) :
	QListView(_parent),
	m_editor(0)
{
	initView();
	initConnections();
}

void ScenarioReviewView::setEditor(ScenarioTextEdit* _editor)
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

void ScenarioReviewView::resizeEvent(QResizeEvent* _event)
{
	QListView::resizeEvent(_event);

	reset();
}

void ScenarioReviewView::keyPressEvent(QKeyEvent* _event)
{
	//
	// Отлавливаем необходимую комбинацию клавиш
	//
	const QString keyCharacter = _event->text();
	if (_event->modifiers().testFlag(Qt::ControlModifier)
		&& ((Qt::Key)_event->key() == Qt::Key_Z
			|| keyCharacter == "z"
			|| keyCharacter == QString::fromUtf8("я"))) {
		if (_event->modifiers().testFlag(Qt::ShiftModifier)) {
			emit redoPressed();
		} else {
			emit undoPressed();
		}
	}

	QListView::keyPressEvent(_event);
}

void ScenarioReviewView::aboutUpdateModel()
{
	if (m_editor != 0) {
		if (ScenarioTextDocument* document = qobject_cast<ScenarioTextDocument*>(m_editor->document())) {
			ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(document->reviewModel());
			setModel(reviewModel);

			connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(aboutEdit(QModelIndex)), Qt::UniqueConnection);
		}
	}
}

void ScenarioReviewView::aboutMoveCursorToMark(const QModelIndex& _index)
{
	if (_index.isValid()) {
		//
		// Отключаемся, чтобы не моргал выделенный элемент
		//
		disconnect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutSelectMark()));

		if (ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model())) {
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
}

void ScenarioReviewView::aboutSelectMark()
{
	const int cursorPosition = m_editor->textCursor().position();
	if (ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model())) {
		const QModelIndex index = reviewModel->indexForPosition(cursorPosition);
		clearSelection();
		setCurrentIndex(index);
		if (index.isValid()) {
			scrollTo(index);
		}
	}
}

void ScenarioReviewView::aboutContextMenuRequested(const QPoint& _pos)
{
	QMenu* menu = new QMenu(this);
	QAction* edit = menu->addAction(tr("Edit"));
	QAction* reply = menu->addAction(tr("Reply"));
	QAction* done = menu->addAction(tr("Done"));
	done->setCheckable(true);
	done->setChecked(model()->data(currentIndex(), ScenarioReviewModel::IsDoneRole).toBool());
	menu->addSeparator();
	QAction* remove = menu->addAction(tr("Remove"));

	//
	// Определим комментарий над коротым нажата кнопка мыши
	//
	const int y = _pos.y() - visualRect(currentIndex()).top();
	const int commentIndex = ScenarioReviewItemDelegate::commentIndexFor(currentIndex(), y, this);

	QAction* toggled = menu->exec(mapToGlobal(_pos));
	if (toggled != 0) {
		const QModelIndex lastCurrent = currentIndex();
		if (toggled == edit) {
			aboutEdit(commentIndex);
		} else if (toggled == reply) {
			aboutReply();
		} else if (toggled == done) {
			aboutDone(done->isChecked());
		} else if (toggled == remove) {
			aboutDelete(commentIndex);
		}
		clearSelection();
		setCurrentIndex(lastCurrent);
	}

	menu->deleteLater();
}

void ScenarioReviewView::aboutEdit(int _commentIndex)
{
	if (currentIndex().isValid()) {
		ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
		const QString oldComment =
			reviewModel->data(
				currentIndex(),
				ScenarioReviewModel::CommentsRole
				).toStringList().value(_commentIndex);
		const QString comment =
			QLightBoxInputDialog::getLongText(this, QString::null, tr("Comment"), oldComment);
		if (!comment.isEmpty()) {
			reviewModel->updateReviewMarkComment(currentIndex(), _commentIndex, comment);
		}
	}
}

void ScenarioReviewView::aboutEdit(const QModelIndex& _index)
{
	if (_index.isValid()) {
		const int y = mapFromGlobal(QCursor::pos()).y() - visualRect(_index).top();
		const int commentIndex = ScenarioReviewItemDelegate::commentIndexFor(_index, y, this);
		aboutEdit(commentIndex);
	}
}

void ScenarioReviewView::aboutReply()
{
	if (currentIndex().isValid()) {
		const QString comment = QLightBoxInputDialog::getLongText(this, QString::null, tr("Reply"));
		if (!comment.isEmpty()) {
			ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
			reviewModel->addReviewMarkComment(currentIndex(), comment);
		}
	}
}

void ScenarioReviewView::aboutDone(bool _done)
{
	if (currentIndex().isValid()) {
		ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
		reviewModel->setReviewMarkIsDone(currentIndex(), _done);
	}
}

void ScenarioReviewView::aboutDelete(int _commentIndex)
{
	if (currentIndex().isValid()) {
		ScenarioReviewModel* reviewModel = qobject_cast<ScenarioReviewModel*>(model());
		reviewModel->removeMark(currentIndex(), _commentIndex);
	}
}

void ScenarioReviewView::initView()
{
	setContextMenuPolicy(Qt::CustomContextMenu);

	setItemDelegate(new ScenarioReviewItemDelegate(this));
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setResizeMode(QListView::Adjust);
}

void ScenarioReviewView::initConnections()
{
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(aboutContextMenuRequested(QPoint)));

	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(aboutMoveCursorToMark(QModelIndex)));
	connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(aboutMoveCursorToMark(QModelIndex)));
}
