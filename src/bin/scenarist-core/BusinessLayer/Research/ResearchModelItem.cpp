#include "ResearchModelItem.h"

#include <Domain/Research.h>

using BusinessLogic::ResearchModelItem;
using Domain::Research;


ResearchModelItem::ResearchModelItem(Domain::Research* _research) :
	m_research(_research),
	m_parent(0)
{
}

ResearchModelItem::~ResearchModelItem()
{
	m_research = 0;
	qDeleteAll(m_children);
}

QString ResearchModelItem::name() const
{
	return
			m_research == 0
			? QString::null
			: m_research->name();
}

QPixmap ResearchModelItem::icon() const
{
	QString iconPath;
	if (m_research != 0) {
		switch (m_research->type()) {
			case Research::Scenario: {
				iconPath = ":/Graphics/Icons/script1.png";
				break;
			}

			case Research::ResearchRoot: {
				iconPath = ":/Graphics/Icons/research1.png";
				break;
			}

			case Research::Folder: {
				iconPath = ":/Graphics/Icons/folder.png";
				break;
			}

			case Research::Text: {
				iconPath = ":/Graphics/Icons/file-document.png";
				break;
			}

			case Research::Url: {
				iconPath = ":/Graphics/Icons/file-web.png";
				break;
			}

			case Research::ImagesGallery: {
				iconPath = ":/Graphics/Icons/file-images.png";
				break;
			}

			case Research::Image: {
				iconPath = ":/Graphics/Icons/file-image.png";
				break;
			}

			default: {
				iconPath = ":/Graphics/Icons/file.png";
				break;
			}
		}
	}

	return QPixmap(iconPath);
}

Domain::Research* ResearchModelItem::research() const
{
	return m_research;
}

//! Вспомогательные методы для организации работы модели

void ResearchModelItem::prependItem(ResearchModelItem* _item)
{
	//
	// Устанавливаем себя родителем
	//
	_item->m_parent = this;

	//
	// Добавляем элемент в список детей
	//
	m_children.prepend(_item);
}

void ResearchModelItem::appendItem(ResearchModelItem* _item)
{
	//
	// Устанавливаем себя родителем
	//
	_item->m_parent = this;

	//
	// Добавляем элемент в список детей
	//
	m_children.append(_item);
}

void ResearchModelItem::insertItem(int _index, ResearchModelItem* _item)
{
	_item->m_parent = this;
	m_children.insert(_index, _item);
}

void ResearchModelItem::removeItem(ResearchModelItem* _item)
{
	//
	// removeOne - удаляет объект при помощи delete, так что потом самому удалять не нужно
	//
	m_children.removeOne(_item);
	_item = 0;
}

bool ResearchModelItem::hasParent() const
{
	return m_parent != 0;
}

ResearchModelItem* ResearchModelItem::parent() const
{
	return m_parent;
}

ResearchModelItem* ResearchModelItem::childAt(int _index) const
{
	return m_children.value(_index, 0);
}

int ResearchModelItem::rowOfChild(ResearchModelItem* _child) const
{
	return m_children.indexOf(_child);
}

int ResearchModelItem::childCount() const
{
	return m_children.count();
}

bool ResearchModelItem::hasChildren() const
{
	return !m_children.isEmpty();
}
