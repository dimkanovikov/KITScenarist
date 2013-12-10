#include "ScenarioTextBlockStyle.h"
#include "ScenarioTextBlockStylePrivate.h"


ScenarioTextBlockStyle::ScenarioTextBlockStyle(ScenarioTextBlockStyle::Type _blockType) :
	m_pimpl(new ScenarioTextBlockStylePrivate(_blockType))
{
	//
	// Запомним в стиле его тип
	//
	m_pimpl->blockFormat.setProperty(ScenarioTextBlockStyle::PropertyType, blockType());
	m_pimpl->charFormat.setProperty(ScenarioTextBlockStyle::PropertyIsFirstUppercase, true);
	m_pimpl->charFormat.setProperty(ScenarioTextBlockStyle::PropertyIsCanModify, true);

	//
	// Настроим остальные характеристики
	//
	switch (blockType()) {
		case SceneHeader:
			m_pimpl->blockFormat.setTopMargin(25);
			m_pimpl->blockFormat.setBackground(QColor("lightGray"));
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case Action:
			m_pimpl->blockFormat.setTopMargin(20);
			break;
		case Character:
			m_pimpl->blockFormat.setTopMargin(15);
			m_pimpl->blockFormat.setLeftMargin(200);
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case Parenthetical:
			m_pimpl->blockFormat.setLeftMargin(150);
			m_pimpl->blockFormat.setRightMargin(150);
			m_pimpl->charFormat.setProperty(ScenarioTextBlockStyle::PropertyIsFirstUppercase, false);
			m_pimpl->charFormat.setProperty(ScenarioTextBlockStyle::PropertyPrefix, "(");
			m_pimpl->charFormat.setProperty(ScenarioTextBlockStyle::PropertyPostfix, ")");
			break;
		case Dialog:
			m_pimpl->blockFormat.setLeftMargin(100);
			m_pimpl->blockFormat.setRightMargin(100);
			break;
		case Transition:
			m_pimpl->blockFormat.setAlignment(Qt::AlignRight);
			m_pimpl->charFormat.setFontCapitalization(QFont::AllUppercase);
			break;
		case NoteText:
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
	return m_pimpl->charFormat.boolProperty(ScenarioTextBlockStyle::PropertyIsFirstUppercase);
}

bool ScenarioTextBlockStyle::isCanModify() const
{
	return m_pimpl->charFormat.boolProperty(ScenarioTextBlockStyle::PropertyIsCanModify);
}

bool ScenarioTextBlockStyle::hasDecoration() const
{

	return !prefix().isEmpty()
			|| !postfix().isEmpty();
}

QString ScenarioTextBlockStyle::prefix() const
{
	return m_pimpl->charFormat.stringProperty(ScenarioTextBlockStyle::PropertyPrefix);
}

QString ScenarioTextBlockStyle::postfix() const
{
	return m_pimpl->charFormat.stringProperty(ScenarioTextBlockStyle::PropertyPostfix);
}
