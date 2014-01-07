#include "CompletableTextEdit.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTextBlock>


CompletableTextEdit::CompletableTextEdit(QWidget* _parent) :
	SpellCheckTextEdit(_parent), m_completer(0)
{
	m_completer = new QCompleter(this);
	m_completer->setWidget(this);
	m_completer->setWrapAround(false);
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
			rect.setWidth(
						m_completer->popup()->sizeHintForColumn(0)
						+ m_completer->popup()->verticalScrollBar()->sizeHint().width());
			m_completer->complete(rect);

			success = true;
		}
	}

	if (!success) {
		//
		// ... скроем, если был отображён
		//
		closeCompleter();
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

		//
		// Вставим дополнение в текст
		//
		int completionStartFrom = m_completer->completionPrefix().length();
		QString textToInsert = completion.mid(completionStartFrom);

		if (!textCursor().block().text().isEmpty()) {
			forever {
				QTextCursor cursor = textCursor();
				cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
				if (cursor.selectedText().endsWith(m_completer->completionPrefix())) {
					textCursor().insertText(textToInsert);
					break;
				}
				moveCursor(QTextCursor::Right);
			}
		} else {
			textCursor().insertText(textToInsert);
		}

		closeCompleter();
	}
}

void CompletableTextEdit::closeCompleter()
{
	m_completer->popup()->hide();
}
