#include "SpellCheckHighlighter.h"
#include "SpellChecker.h"

#include <QTextDocument>


SpellCheckHighlighter::SpellCheckHighlighter(QTextDocument* _parent, SpellChecker* _checker) :
	SyntaxHighlighter(_parent),
	m_spellchecker(_checker),
	m_useSpellChecker(true)
{
	Q_ASSERT(_checker);

	//
	// Настроим стиль выделения текста не прошедшего проверку
	//
	m_misspeledCharFormat.setUnderlineStyle(QTextCharFormat::DashUnderline);
	m_misspeledCharFormat.setUnderlineColor(Qt::red);
}

void SpellCheckHighlighter::setUseSpellChecker(bool _use)
{
	m_useSpellChecker = _use;

    if (document() != 0
            && !document()->isEmpty()) {
		rehighlight();
	}
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
						const int wordWithoutPunctLength = wordWithoutPunct.length();
						int positionInText = _text.indexOf(wordWithoutPunct, positionInText);
						while (positionInText != -1) {
							setFormat(positionInText, wordWithoutPunctLength, m_misspeledCharFormat);
							positionInText = _text.indexOf(wordWithoutPunct, positionInText + 1);
						}
					}
				}
			}
		}
	}
}
