#include "SpellCheckHighlighter.h"
#include "SpellChecker.h"

#include <QFile>
#include <QDir>
#include <QUuid>
#include <QTemporaryFile>


SpellCheckHighlighter::SpellCheckHighlighter(QTextDocument* _parent, SpellChecker* _checker) :
	QSyntaxHighlighter(_parent),
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

	if (document() != 0 && _use) {
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
					wordWithoutPunct = wordWithoutPunct[0] + wordWithoutPunct.mid(1).toLower();

					//
					// Если слово не прошло проверку
					//
					if (!m_spellchecker->spellCheckWord(wordWithoutPunct)) {
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
