#include "ScenarioTextBlockStyle.h"
#include "ScenarioTextBlockStylePrivate.h"


ScenarioTextBlockStyle::ScenarioTextBlockStyle(ScenarioTextBlockStyle::Type _blockType) :
	m_pimpl(new ScenarioTextBlockStylePrivate(_blockType))
{
	//
	// Запомним в стиле его тип
	//
	m_pimpl->blockFormat.setProperty(QTextFormat::UserProperty, blockType());

	//
	// Настроим остальные характеристики
	//
	switch (blockType()) {
		case SceneHeader:
			m_pimpl->blockFormat.setTopMargin(15);
			m_pimpl->blockFormat.setBottomMargin(15);
			m_pimpl->blockFormat.setBackground(QColor("lightGray"));
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case Action:
			m_pimpl->blockFormat.setTopMargin(15);
			m_pimpl->blockFormat.setBottomMargin(15);
			break;
		case Character:
			m_pimpl->blockFormat.setTopMargin(15);
			m_pimpl->blockFormat.setLeftMargin(200);
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case Parenthetical:
			m_pimpl->blockFormat.setLeftMargin(150);
			m_pimpl->blockFormat.setRightMargin(150);
			m_pimpl->isFirstUppercase = false;
			m_pimpl->prefix = "(";
			m_pimpl->postfix = ")";
			break;
		case Dialog:
			m_pimpl->blockFormat.setLeftMargin(100);
			m_pimpl->blockFormat.setRightMargin(100);
			m_pimpl->blockFormat.setBottomMargin(15);
			break;
		case Transition:
			m_pimpl->blockFormat.setAlignment(Qt::AlignRight);
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case OtherText:
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case TitleHeader:
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case Title:
			m_pimpl->blockFormat.setLeftMargin(150);
			m_pimpl->blockFormat.setRightMargin(150);
			break;
		case SceneGroupHeader:
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case SceneGroupFooter:
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case SimpleText:
			break;
		default:
			break;
	}
}

ScenarioTextBlockStyle::~ScenarioTextBlockStyle()
{
	delete m_pimpl;
}

ScenarioTextBlockStyle::Type ScenarioTextBlockStyle::blockType() const
{
	return m_pimpl->blockType;
}

QTextBlockFormat ScenarioTextBlockStyle::blockFormat() const
{
	return m_pimpl->blockFormat;
}

QTextCharFormat ScenarioTextBlockStyle::charFormat() const
{
	return m_pimpl->charFormat;
}

bool ScenarioTextBlockStyle::isFirstUppercase() const
{
	return m_pimpl->isFirstUppercase;
}

bool ScenarioTextBlockStyle::isCanModify() const
{
	return m_pimpl->isCanModify;
}

bool ScenarioTextBlockStyle::hasDecoration() const
{
	return !prefix().isEmpty()
			|| !postfix().isEmpty();
}

QString ScenarioTextBlockStyle::prefix() const
{
	return m_pimpl->prefix;
}

QString ScenarioTextBlockStyle::postfix() const
{
	return m_pimpl->postfix;
}
