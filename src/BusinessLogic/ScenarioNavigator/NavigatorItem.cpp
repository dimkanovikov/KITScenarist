#include "NavigatorItem.h"

#include <QTextDocument>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>
#include <BusinessLogic/Chronometry/ChronometerFacade.h>


NavigatorItem::NavigatorItem() :
	m_parent(0)
{
}

NavigatorItem::~NavigatorItem()
{
	qDeleteAll(m_children);
}

NavigatorItem* NavigatorItem::parent() const
{
	return m_parent;
}

NavigatorItem* NavigatorItem::childAt(int _index) const
{
	return m_children.value(_index, 0);
}

int NavigatorItem::rowOfChild(NavigatorItem* _child) const
{
	return m_children.indexOf(_child);
}

int NavigatorItem::childCount() const
{
	return m_children.count();
}

QPixmap NavigatorItem::icon() const
{
	QPixmap icon;

	if (isFolder()) {
		icon = QPixmap(":/Graphics/Icons/folder.png");
	} else {
		icon = QPixmap(":/Graphics/Icons/scene.png");
	}

	return icon;
}

QString NavigatorItem::header() const
{
	return m_headerBlock.text();
}

QString NavigatorItem::description() const
{
	QString description;

	if (m_headerBlock != m_endBlock) {
		QTextBlock descriptionBuilder = m_headerBlock;
		do {
			descriptionBuilder = descriptionBuilder.next();
			if (!description.isEmpty()) {
				description += " ";
			}
			description += descriptionBuilder.text();
		} while (descriptionBuilder != m_endBlock
				 && descriptionBuilder.isValid());
	}

	return description;
}

int NavigatorItem::timing() const
{
	return ChronometerFacade::calculate(m_headerBlock, m_endBlock);
}

QTextBlock NavigatorItem::headerBlock() const
{
	return m_headerBlock;
}

QTextBlock NavigatorItem::endBlock() const
{
	return m_endBlock;
}

bool NavigatorItem::isFolder() const
{
	return ScenarioTextBlockStyle::forBlock(m_headerBlock) == ScenarioTextBlockStyle::FolderHeader;
}

void NavigatorItem::setHeaderBlock(const QTextBlock& _block)
{
	if (m_headerBlock != _block) {
		m_headerBlock = _block;
	}
}

void NavigatorItem::setEndBlock(const QTextBlock& _block)
{
	if (m_endBlock != _block) {
		m_endBlock = _block;
	}
}

void NavigatorItem::append(NavigatorItem* _item)
{
	if (!m_children.contains(_item)) {
		m_children.append(_item);
		_item->m_parent = this;
	}
}
