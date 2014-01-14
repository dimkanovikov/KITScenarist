#include "NavigatorItem.h"

#include <QTextDocument>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>
#include <BusinessLogic/Chronometry/ChronometerFacade.h>


NavigatorItem::NavigatorItem() :
	m_parent(0),
	m_timing(0)
{
}

NavigatorItem::~NavigatorItem()
{
	qDeleteAll(m_children);
}

bool NavigatorItem::hasParent() const
{
	return m_parent != 0;
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
	return m_header;
}

QString NavigatorItem::description() const
{
	return m_description;
}

int NavigatorItem::timing() const
{
	return m_timing;
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

		updateItem();
	}
}

void NavigatorItem::updateItem()
{
	//
	// Обновим заголовок
	//
	m_header = m_headerBlock.text();

	//
	// Обновим описание
	//
	m_description.clear();
	if (m_headerBlock != m_endBlock) {
		QTextBlock descriptionBuilder = m_headerBlock;
		do {
			descriptionBuilder = descriptionBuilder.next();
			if (!m_description.isEmpty()) {
				m_description += " ";
			}
			m_description += descriptionBuilder.text();
		} while (descriptionBuilder != m_endBlock
				 && descriptionBuilder.isValid()
				 && m_description.length() < 200);
	}

	//
	// Обновим хронометраж
	//
	if (isFolder()) {
		m_timing = 0;
		foreach (NavigatorItem* child, m_children) {
			m_timing += child->timing();
		}
	} else {
		m_timing = ChronometerFacade::calculate(m_headerBlock, m_endBlock);
	}
}

void NavigatorItem::append(NavigatorItem* _item)
{
	if (!m_children.contains(_item)) {
		m_children.append(_item);
		_item->m_parent = this;
	}
}
