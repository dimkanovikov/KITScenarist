#include "SpellCheckHighlighter.h"
#include "SpellChecker.h"

#include <QFile>
#include <QDir>
#include <QUuid>
#include <QTemporaryFile>


SpellCheckHighlighter::SpellCheckHighlighter(QTextDocument* _parent, SpellChecker* _checker) :
	QSyntaxHighlighter(_parent),
	m_spellchecker(_checker)
{
	Q_ASSERT(_parent);
	Q_ASSERT(_checker);

	//
	// Настроим стиль выделения текста не прошедшего проверку
	//
	m_misspeledCharFormat.setUnderlineStyle(QTextCharFormat::DashUnderline);
	m_misspeledCharFormat.setUnderlineColor(Qt::red);
}

void SpellCheckHighlighter::highlightBlock(const QString& _text)
{
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
			// Проверяем слова длинной более одного символа
			// и не начинающиеся с символа "\"
			//
			if (wordToCheck.length() > 1
				&& !wordToCheck.startsWith('\\')) {
				//
				// Если слово не прошло проверку
				//
				if (!m_spellchecker->spellCheckWord(wordToCheck)) {
					//
					// Проходим по всем вхождения этого слова в тексте
					//
					int wordsCount = _text.count(QRegExp("\\b" + wordToCheck + "\\b"));
					int positionInText = -1;

					for (int wordIndex = 0; wordIndex < wordsCount; ++wordIndex) {
						positionInText =
								_text.indexOf(QRegExp("\\b" + wordToCheck + "\\b"),
											  positionInText + 1);
						//
						// Пометим вхождение слова, как ошибочное
						//
						if (positionInText >= 0) {
							setFormat(positionInText,
									  wordToCheck.length(),
									  m_misspeledCharFormat);
						}
					}
				}
			}
		}
	}
}
