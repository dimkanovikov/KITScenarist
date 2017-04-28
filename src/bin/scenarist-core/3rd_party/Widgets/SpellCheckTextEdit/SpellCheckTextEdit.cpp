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
	m_spellChecker = SpellChecker::createSpellChecker(userDictionaryfile());

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

bool SpellCheckTextEdit::useSpellChecker() const
{
	return m_spellCheckHighlighter->useSpellChecker();
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

SpellChecker::Language SpellCheckTextEdit::spellCheckLanguage() const
{
	return m_spellChecker->spellingLanguage();
}

QMenu* SpellCheckTextEdit::createContextMenu(const QPoint& _pos, QWidget* _parent)
{
	//
	// Запомним позицию курсора
	//
	m_lastCursorPosition = _pos;

	//
	// Сформируем стандартное контекстное меню
	//
	QMenu* menu = createStandardContextMenu(_parent);

	if (m_spellCheckHighlighter->useSpellChecker()) {
		//
		// Определим слово под курсором
		//
		QString wordUnderCursor = wordOnPosition(m_lastCursorPosition);

		//
        // Уберем пунктуацию в слове
		//
        QString wordWithoutPunct = removePunctutaion(wordUnderCursor);

        QString wordWithoutPunctInCorrectRegister;
        if (cursorForPosition(_pos).charFormat().fontCapitalization() == QFont::AllUppercase) {
            //
            // Приведем к верхнему регистру
            //
            wordWithoutPunctInCorrectRegister = wordWithoutPunct.toUpper();
        } else {
            //
            // Приведем к нижнему регистру
            //
            wordWithoutPunctInCorrectRegister = wordWithoutPunct.toLower();
        }

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
					m_suggestionsActions.at(addedSuggestionsCount)->setEnabled(true);
					menu->insertAction(actionInsertBefore, m_suggestionsActions.at(addedSuggestionsCount));
					++addedSuggestionsCount;
				} else {
					break;
				}
			}
			if (addedSuggestionsCount == 0) {
				m_suggestionsActions.first()->setText(tr("Suggestions not found"));
				m_suggestionsActions.first()->setEnabled(false);
				menu->insertAction(actionInsertBefore, m_suggestionsActions.first());
			}
			menu->insertSeparator(actionInsertBefore);
			// ... вставляем дополнительные действия
			menu->insertAction(actionInsertBefore, m_ignoreWordAction);
			menu->insertAction(actionInsertBefore, m_addWordToUserDictionaryAction);
			menu->insertSeparator(actionInsertBefore);
		}
		//
		// TODO: тезаурус готов, нужны доработки по визуализации и по качеству самой базы слов
		//
		//
		// Если проходит проверку, то добавляем вхождения из тезауруса
		//
//		else {
//			QMap<QString, QSet<QString> > thesaurusEntries =
//				m_spellChecker->synonimsForWord(wordWithoutPunct.toLower());
//			QMenu* thesaurusMenu = new QMenu(menu);
//			//
//			// Если есть вхождения, то показываем меню с ними
//			//
//			if (!thesaurusEntries.isEmpty()) {
//				QList<QString> entries = thesaurusEntries.keys();
//				qSort(entries);
//				foreach (const QString& entry, entries) {
//					QMenu* entryMenu = new QMenu(menu);
//					QList<QString> entryItems = thesaurusEntries.value(entry).toList();
//					qSort(entryItems);
//					foreach (const QString& entryItem, entryItems) {
//						entryMenu->addAction(entryItem);
//					}
//					QAction* entryAction = thesaurusMenu->addAction(entry);
//					entryAction->setMenu(entryMenu);
//				}
//			}
//			//
//			// Если нет вхождений, то так и говорим
//			//
//			else {
//				QAction* entryAction = thesaurusMenu->addAction(tr("No available information."));
//				entryAction->setEnabled(false);
//			}

//			QAction* thesaurusAction =
//				new QAction(tr("%1 in dictionary").arg(wordWithoutPunct), menu);
//			thesaurusAction->setMenu(thesaurusMenu);
//			QAction* actionInsertBefore = menu->actions().first();
//			menu->insertAction(actionInsertBefore, thesaurusAction);
//			menu->insertSeparator(actionInsertBefore);
//		}
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
    // Уберем пунктуацию
    //
    QString wordUnderCursorWithoutPunct = removePunctutaion(wordUnderCursor);

	//
	// Скорректируем регистр слова
	//
    QString wordUnderCursorWithoutPunctInCorrectRegister = wordUnderCursorWithoutPunct.toLower();

	//
	// Объявляем проверяющему о том, что это слово нужно игнорировать
	//
    m_spellChecker->ignoreWord(wordUnderCursorWithoutPunctInCorrectRegister);

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
    // Уберем пунктуацию в слове
	//
    QString wordUnderCursorWithoutPunct = removePunctutaion(wordUnderCursor);

    //
    // Приведем к нижнему регистру
    //
    QString wordUnderCursorWithoutPunctInCorrectRegister = wordUnderCursorWithoutPunct.toLower();

	//
	// Объявляем проверяющему о том, что это слово нужно добавить в пользовательский словарь
	//
    m_spellChecker->addWordToDictionary(wordUnderCursorWithoutPunctInCorrectRegister);

	//
	// Уберём выделение со слов добавленных в словарь
	//
	m_spellCheckHighlighter->rehighlight();
}

void SpellCheckTextEdit::aboutReplaceWordOnSuggestion()
{
	if (QAction* suggestAction = qobject_cast<QAction*>(sender())) {
		QTextCursor cursor = cursorForPosition(m_lastCursorPosition);
		cursor.beginEditBlock();
		cursor.select(QTextCursor::WordUnderCursor);
		cursor.removeSelectedText();
		cursor.insertText(suggestAction->text());
		setTextCursor(cursor);
		cursor.endEditBlock();
	}
}

QString SpellCheckTextEdit::wordOnPosition(const QPoint& _position) const
{
	QTextCursor tc = cursorForPosition(_position);
	tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QString SpellCheckTextEdit::removePunctutaion(const QString &_word) const
{
    //
    // Убираем знаки препинания окружающие слово
    //
    QString wordWithoutPunct = _word.trimmed();
    while (!wordWithoutPunct.isEmpty()
           && (wordWithoutPunct.at(0).isPunct()
               || wordWithoutPunct.at(wordWithoutPunct.length()-1).isPunct())) {
        if (wordWithoutPunct.at(0).isPunct()) {
            wordWithoutPunct = wordWithoutPunct.mid(1);
        } else {
            wordWithoutPunct = wordWithoutPunct.left(wordWithoutPunct.length()-1);
        }
    }
    return wordWithoutPunct;
}
