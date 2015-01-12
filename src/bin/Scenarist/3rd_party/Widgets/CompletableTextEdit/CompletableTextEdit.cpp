#include "CompletableTextEdit.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QEvent>
#include <QScrollBar>

namespace {
	//
	// NOTE: При переходе от обычного редактора к обёртке масштабирования, дополняльщик
	//		 стал показываться и скрываться через раз после нажатия клавиш. А всё дело в том,
	//		 что его стандартное таково, что если виджет потерял фокус, то всплывающий список
	//		 принудительно скрывается. Приходится применять мини хак.
	//
	class MyCompleter : public QCompleter
	{
	public:
		explicit MyCompleter(QObject* _p = 0) : QCompleter(_p) {}

	protected:
		bool eventFilter(QObject *o, QEvent *e) {
			//
			// Проверяем тип события, чтобы не войти в бесконечную рекурсию
			//
			if (e->type() != QEvent::FocusIn && widget()) {
				widget()->setFocus();
			}
			return QCompleter::eventFilter(o,e);
		}
	};
}


CompletableTextEdit::CompletableTextEdit(QWidget* _parent) :
	SpellCheckTextEdit(_parent), m_completer(new MyCompleter(this))
{
	m_completer->setWidget(this);
	connect(m_completer, SIGNAL(activated(QString)), this, SLOT(applyCompletion(QString)));
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
			rect.moveTo(viewport()->mapTo(this, rect.topLeft()));
			rect.setX(rect.x() + verticalScrollBar()->width());
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
		moveCursor(QTextCursor::Right);
	}

	textCursor().insertText(textToInsert);
}

void CompletableTextEdit::closeCompleter()
{
	m_completer->popup()->hide();
}
