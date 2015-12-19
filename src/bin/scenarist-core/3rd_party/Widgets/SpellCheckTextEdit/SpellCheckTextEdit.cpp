#include "SpellCheckTextEdit.h"
#include "SpellChecker.h"
#include "SpellCheckHighlighter.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QDir>
#include <QMenu>
#include <QStandardPaths>


namespace {
	/**
	 * @brief Максимальное кол-во подсказок для проверки орфографии
	 */
	const int SUGGESTIONS_ACTIONS_MAX_COUNT = 5;
}

SpellCheckTextEdit::SpellCheckTextEdit(QWidget *_parent) :
	PageTextEdit(_parent),
	m_spellChecker(0),
	m_spellCheckHighlighter(0)
{
	//
	// Настроим проверяющего
	//
	m_spellChecker = new SpellChecker(userDictionaryfile());

	//
	// Настраиваем подсветку слов не прошедших проверку орфографии
	//
	m_spellCheckHighlighter = new SpellCheckHighlighter(0, m_spellChecker);

	//
	// Настраиваем действия контекстного меню для слов не прошедших проверку орфографии
	//
	// ... игнорировать слово
	m_ignoreWordAction = new QAction(tr("Ignore"), this);
	connect(m_ignoreWordAction, SIGNAL(triggered()), this, SLOT(aboutIgnoreWord()));
	// ... добавить слово в словарь
	m_addWordToUserDictionaryAction = new QAction(tr("Add to dictionary"), this);
	connect(m_addWordToUserDictionaryAction, SIGNAL(triggered()),
			this, SLOT(aboutAddWordToUserDictionary()));
	// ... добавляем несколько пустых пунктов, для последующего помещения в них вариантов
	for (int actionIndex = 0; actionIndex < SUGGESTIONS_ACTIONS_MAX_COUNT; ++actionIndex) {
		m_suggestionsActions.append(new QAction(QString(), this));
		connect(m_suggestionsActions.at(actionIndex), SIGNAL(triggered()),
				this, SLOT(aboutReplaceWordOnSuggestion()));
	}
}

void SpellCheckTextEdit::setUseSpellChecker(bool _use)
{
	m_spellCheckHighlighter->setUseSpellChecker(_use);
}

void SpellCheckTextEdit::setSpellCheckLanguage(SpellChecker::Language _language)
{
	if (m_spellChecker->spellingLanguage() != _language) {
		//
		// Установим язык проверяющего
		//
		m_spellChecker->setSpellingLanguage(_language);

		//
		// Заново выделим слова не проходящие проверку орфографии вновь заданного языка
		//
		m_spellCheckHighlighter->rehighlight();
	}
}

QMenu* SpellCheckTextEdit::createContextMenu(const QPoint& _pos)
{
	//
	// Запомним позицию курсора
	//
	m_lastCursorPosition = _pos;

	//
	// Сформируем стандартное контекстное меню
	//
	QMenu* menu = createStandardContextMenu();

	if (m_spellCheckHighlighter->useSpellChecker()) {
		//
		// Определим слово под курсором
		//
		QString wordUnderCursor = wordOnPosition(m_lastCursorPosition);

		//
		// Убираем знаки препинания окружающие слово
		//
		QString wordWithoutPunct = wordUnderCursor.trimmed();
		while (!wordWithoutPunct.isEmpty()
			   && (wordWithoutPunct.at(0).isPunct()
				   || wordWithoutPunct.at(wordWithoutPunct.length()-1).isPunct())) {
			if (wordWithoutPunct.at(0).isPunct()) {
				wordWithoutPunct = wordWithoutPunct.mid(1);
			} else {
				wordWithoutPunct = wordWithoutPunct.left(wordWithoutPunct.length()-1);
			}
		}

		//
		// Корректируем регистр слова
		//
		QString wordWithoutPunctInCorrectRegister =
				wordWithoutPunct[0] + wordWithoutPunct.mid(1).toLower();

		//
		// Если слово не проходит проверку орфографии добавим дополнительные действия в контекстное меню
		//
		if (!m_spellChecker->spellCheckWord(wordWithoutPunctInCorrectRegister)) {
			// ... действие, перед которым вставляем дополнительные пункты
			QStringList suggestions = m_spellChecker->suggestionsForWord(wordWithoutPunct);
			// ... вставляем варианты
			QAction* actionInsertBefore = menu->actions().first();
			int addedSuggestionsCount = 0;
			foreach (const QString& suggestion, suggestions) {
				if (addedSuggestionsCount < SUGGESTIONS_ACTIONS_MAX_COUNT) {
					m_suggestionsActions.at(addedSuggestionsCount)->setText(suggestion);
					menu->insertAction(actionInsertBefore, m_suggestionsActions.at(addedSuggestionsCount));
					++addedSuggestionsCount;
				} else {
					break;
				}
			}
			if (addedSuggestionsCount > 0) {
				menu->insertSeparator(actionInsertBefore);
			}
			// ... вставляем дополнительные действия
			menu->insertAction(actionInsertBefore, m_ignoreWordAction);
			menu->insertAction(actionInsertBefore, m_addWordToUserDictionaryAction);
			menu->insertSeparator(actionInsertBefore);
		}
	}

	return menu;
}

QString SpellCheckTextEdit::userDictionaryfile() const
{
	QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString hunspellDictionariesFolderPath = appDataFolderPath + QDir::separator() + "Hunspell";
	QString dictionaryFilePath = hunspellDictionariesFolderPath + QDir::separator() + "UserDictionary.dict";
	return dictionaryFilePath;
}

void SpellCheckTextEdit::contextMenuEvent(QContextMenuEvent* _event)
{
	QMenu* contextMenu = createContextMenu(_event->pos());

	//
	// Покажем меню, а после очистим от него память
	//
	contextMenu->exec(_event->globalPos());
	delete contextMenu;
}

void SpellCheckTextEdit::setHighlighterDocument(QTextDocument* _document)
{
	m_spellCheckHighlighter->setDocument(_document);
}

void SpellCheckTextEdit::aboutIgnoreWord() const
{
	//
	// Определим слово под курсором
	//
	QString wordUnderCursor = wordOnPosition(m_lastCursorPosition);

	//
	// Скорректируем регистр слова
	//
	QString wordUnderCursorInCorrectRegister =
			wordUnderCursor[0] + wordUnderCursor.mid(1).toLower();

	//
	// Объявляем проверяющему о том, что это слово нужно игнорировать
	//
	m_spellChecker->ignoreWord(wordUnderCursorInCorrectRegister);

	//
	// Уберём выделение с игнорируемых слов
	//
	m_spellCheckHighlighter->rehighlight();
}

void SpellCheckTextEdit::aboutAddWordToUserDictionary() const
{
	//
	// Определим слово под курсором
	//
	QString wordUnderCursor = wordOnPosition(m_lastCursorPosition);

	//
	// Скорректируем регистр слова
	//
	QString wordUnderCursorInCorrectRegister =
			wordUnderCursor[0] + wordUnderCursor.mid(1).toLower();

	//
	// Объявляем проверяющему о том, что это слово нужно добавить в пользовательский словарь
	//
	m_spellChecker->addWordToDictionary(wordUnderCursorInCorrectRegister);

	//
	// Уберём выделение со слов добавленных в словарь
	//
	m_spellCheckHighlighter->rehighlight();
}

void SpellCheckTextEdit::aboutReplaceWordOnSuggestion() const
{
	if (QAction* suggestAction = qobject_cast<QAction*>(sender())) {
		QTextCursor tc = cursorForPosition(m_lastCursorPosition);
		tc.select(QTextCursor::WordUnderCursor);
		tc.removeSelectedText();
		tc.insertText(suggestAction->text());
	}
}

QString SpellCheckTextEdit::wordOnPosition(const QPoint& _position) const
{
	QTextCursor tc = cursorForPosition(_position);
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}
