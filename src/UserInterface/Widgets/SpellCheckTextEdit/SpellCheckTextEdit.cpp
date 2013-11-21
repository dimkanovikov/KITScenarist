#include "SpellCheckTextEdit.h"
#include "SpellChecker.h"
#include "SpellCheckHighlighter.h"


SpellCheckTextEdit::SpellCheckTextEdit(QWidget *parent) :
	QTextEdit(parent)
{
	//
	// Настроим проверяющего
	//
	m_spellChecker = new SpellChecker(userDictionaryfile());

	//
	// Настраиваем подсветку слов не прошедших проверку орфографии
	//
	m_spellCheckHighlighter = new SpellCheckHighlighter(document(), m_spellChecker);
}

QString SpellCheckTextEdit::userDictionaryfile() const
{
	return QString::null;
}
