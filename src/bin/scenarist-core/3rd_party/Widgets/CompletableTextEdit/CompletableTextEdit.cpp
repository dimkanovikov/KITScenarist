#include "CompletableTextEdit.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QEvent>
#include <QScrollBar>

namespace {
	class MyCompleter : public QCompleter
	{
	public:
		explicit MyCompleter(QObject* _p = 0) : QCompleter(_p) {}

		/**
		 * @brief Переопределяется для отображения подсказки по глобальной координате
		 *		  левого верхнего угла области для отображения
		 */
		void completeReimpl(const QRect& _rect) {
			complete(_rect);
			popup()->move(_rect.topLeft());
		}
	};
}


CompletableTextEdit::CompletableTextEdit(QWidget* _parent) :
	SpellCheckTextEdit(_parent), m_useCompleter(true), m_completer(new MyCompleter(this))
{
	m_completer->setWidget(this);
	connect(m_completer, SIGNAL(activated(QString)), this, SLOT(applyCompletion(QString)));
}

void CompletableTextEdit::setUseCompleter(bool _use)
{
	if (m_useCompleter != _use) {
		m_useCompleter = _use;
	}
}

QCompleter* CompletableTextEdit::completer() const
{
	return m_completer;
}

bool CompletableTextEdit::isCompleterVisible() const
{
	return m_completer->popup()->isVisible();
}

bool CompletableTextEdit::complete(QAbstractItemModel* _model, const QString& _completionPrefix)
{
	bool success = false;

	if (m_useCompleter && canComplete()) {
		if (_model != 0) {
			//
			// Настроим завершателя, если необходимо
			//
			bool settedNewModel = m_completer->model() != _model;
			bool oldModelWasChanged = false;
			if (!settedNewModel
				&& _model != 0) {
				oldModelWasChanged = m_completer->model()->rowCount() == _model->rowCount();
			}

			if (settedNewModel
				|| oldModelWasChanged) {
				m_completer->setModel(_model);
				m_completer->setModelSorting(QCompleter::UnsortedModel);
				m_completer->setCaseSensitivity(Qt::CaseInsensitive);
			}
			m_completer->setCompletionPrefix(_completionPrefix);

			//
			// Если в модели для дополнения есть элементы
			//
			bool hasCompletions = m_completer->completionModel()->rowCount() > 0;
			bool alreadyComplete = _completionPrefix.toLower().endsWith(m_completer->currentCompletion().toLower());

			if (hasCompletions
				&& !alreadyComplete) {
				m_completer->popup()->setCurrentIndex(
							m_completer->completionModel()->index(0, 0));

				//
				// ... отобразим завершателя
				//
				QRect rect = cursorRect();
				rect.moveTo(mapToGlobal(viewport()->mapToParent(rect.topLeft())));
				rect.moveLeft(rect.left() + verticalScrollBar()->width());
				rect.moveTop(rect.top() + QFontMetricsF(currentCharFormat().font()).height());
				rect.setWidth(
							m_completer->popup()->sizeHintForColumn(0)
							+ m_completer->popup()->verticalScrollBar()->sizeHint().width());

				MyCompleter* myCompleter = static_cast<MyCompleter*>(m_completer);
				myCompleter->completeReimpl(rect);

				success = true;
			}
		}

		if (!success) {
			//
			// ... скроем, если был отображён
			//
			closeCompleter();
		}
	}

	return success;
}

void CompletableTextEdit::applyCompletion()
{
	if (isCompleterVisible()) {
		//
		// Получим выбранный из списка дополнений элемент
		//
		QModelIndex currentIndex = m_completer->popup()->currentIndex();
		QString completion = m_completer->popup()->model()->data(currentIndex).toString();

		applyCompletion(completion);

		closeCompleter();
	}
}

void CompletableTextEdit::applyCompletion(const QString& _completion)
{
	//
	// Вставим дополнение в текст
	//
	int completionStartFrom = m_completer->completionPrefix().length();
	QString textToInsert = _completion.mid(completionStartFrom);

	while (!textCursor().atBlockEnd()) {
		QTextCursor cursor = textCursor();
		cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
		if (cursor.selectedText().endsWith(m_completer->completionPrefix())) {
			break;
		}
		moveCursor(QTextCursor::NextCharacter);
	}

	textCursor().insertText(textToInsert);
}

void CompletableTextEdit::closeCompleter()
{
	m_completer->popup()->hide();
}

bool CompletableTextEdit::canComplete() const
{
	return true;
}
