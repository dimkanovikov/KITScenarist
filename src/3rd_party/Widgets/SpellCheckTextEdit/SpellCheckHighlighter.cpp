#include "SpellCheckHighlighter.h"
#include "SpellChecker.h"

#include <QFile>
#include <QDir>
#include <QUuid>
#include <QTemporaryFile>
#include <QTextCursor>
#include <QTextDocument>


SpellCheckHighlighter::SpellCheckHighlighter(QTextDocument* _parent, SpellChecker* _checker) :
	QObject(0),
	m_document(_parent),
	m_spellchecker(_checker),
	m_useSpellChecker(true),
	m_inHighlight(false)
{
	Q_ASSERT(_checker);

	//
	// Настроим стиль выделения текста не прошедшего проверку
	//
	m_misspeledCharFormat.setUnderlineStyle(QTextCharFormat::DashUnderline);
	m_misspeledCharFormat.setUnderlineColor(Qt::red);

	//
	// Если возможно, подключаемся к сигналу документа об обновлении текста, для его подсветки
	//
	if (document()) {
		connect(document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(highlightBlock(int,int,int)));
	}
}

void SpellCheckHighlighter::setUseSpellChecker(bool _use)
{
	m_useSpellChecker = _use;

	if (document() != 0
		&& !document()->isEmpty()) {
		rehighlight();
	}
}

void SpellCheckHighlighter::rehighlight()
{
	if (m_document) {
		highlightBlock(0, 0, document()->characterCount());
	}
}

void SpellCheckHighlighter::highlightBlock(int _from, int _charsRemoved, int _charsAdded)
{
	Q_UNUSED(_charsRemoved);

	//
	// При необходимости скорректируем значение количества добавленных символов,
	// чтобы в сумме со стартовой позицией они не превышали длину документа
	//
	if (document()->characterCount() <= _from + _charsAdded) {
		_charsAdded = document()->characterCount() - _from - 1;
	}

	if (!m_inHighlight) {
		m_inHighlight = true;

		if (document()) {
			//
			// FIXME: Это конечно плохо, но другого не придумал...
			//
			// Если изменилась папка/группа не нужно пересчитывать весь текст,
			// нужно обновить одну лишь папку/группу
			//
			if (_charsAdded > 200
				&& _charsRemoved > 200) {
				//
				// Сперва проверим начало группирующего элемента
				//
				QTextCursor cursor(document());
				cursor.setPosition(_from);
				cursor.movePosition(QTextCursor::StartOfBlock);
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				m_start = cursor.selectionStart();
				cursor.beginEditBlock();
				highlightBlock(cursor.selectedText());

				//
				// Затем конец
				//
				cursor.setPosition(_from + _charsAdded);
				cursor.movePosition(QTextCursor::StartOfBlock);
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				m_start = cursor.selectionStart();
				highlightBlock(cursor.selectedText());
				cursor.endEditBlock();
			}
			//
			// В противном случае делаем всё по правильному
			//
			else {
				QTextCursor cursor(document());
				cursor.setPosition(_from);
				cursor.movePosition(QTextCursor::StartOfWord);
				cursor.setPosition(_from + _charsAdded, QTextCursor::KeepAnchor);
				cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
				m_start = cursor.selectionStart();
				cursor.beginEditBlock();
				highlightBlock(cursor.selectedText());
				cursor.endEditBlock();
			}
		}

		m_inHighlight = false;
	}
}

QTextDocument* SpellCheckHighlighter::document() const
{
	return m_document;
}

void SpellCheckHighlighter::highlightBlock(const QString& _text)
{
	if (m_useSpellChecker) {
		//
		// Убираем пустоты из проверяемого текста
		//
		QString textToCheck = _text.simplified();
		if (!textToCheck.isEmpty()) {
			//
			// Разбиваем проверяемый текст на слова
			//
			QStringList wordsToCheck =
					textToCheck.split(QRegExp("([^\\w,^\\\\]|(?=\\\\))+"),
									  QString::SkipEmptyParts);
			//
			// Проверяем каждое слово
			//
			foreach (const QString wordToCheck, wordsToCheck) {
				//
				// Убираем знаки препинания окружающие слово
				//
				QString wordWithoutPunct = wordToCheck.trimmed();
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
				// Проверяем слова длинной более одного символа
				//
				if (wordWithoutPunct.length() > 1) {
					//
					// Корректируем регистр слова
					//
					QString wordWithoutPunctInCorrectRegister =
							wordWithoutPunct[0] + wordWithoutPunct.mid(1).toLower();

					//
					// Если слово не прошло проверку
					//
					if (!m_spellchecker->spellCheckWord(wordWithoutPunctInCorrectRegister)) {
						//
						// Проходим по всем вхождения этого слова в тексте
						//
						int wordsCount = _text.count(QRegExp("\\b" + wordWithoutPunct + "\\b"));
						int positionInText = -1;

						for (int wordIndex = 0; wordIndex < wordsCount; ++wordIndex) {
							positionInText =
									_text.indexOf(QRegExp("\\b" + wordWithoutPunct + "\\b"),
												  positionInText + 1);
							//
							// Пометим вхождение слова, как ошибочное
							//
							if (positionInText >= 0) {
								setFormat(positionInText,
										  wordWithoutPunct.length(),
										  m_misspeledCharFormat);
							}
						}
					}
				}
			}
		}
	}
}

void SpellCheckHighlighter::setFormat(int _start, int _count, const QTextCharFormat& _format)
{
	if (document()) {

		QTextCursor cursor(document());
		cursor.setPosition(m_start + _start);
		cursor.setPosition(m_start + _start + _count, QTextCursor::KeepAnchor);
		cursor.mergeCharFormat(_format);
	}
}
