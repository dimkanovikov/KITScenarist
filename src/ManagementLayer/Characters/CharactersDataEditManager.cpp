#include "CharactersDataEditManager.h"

#include <UserInterfaceLayer/Characters/CharactersDataEdit/CharactersDataEdit.h>

using ManagementLayer::CharactersDataEditManager;
using UserInterface::CharactersDataEdit;


CharactersDataEditManager::CharactersDataEditManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_editor(new CharactersDataEdit(_parentWidget))
{
}

QWidget*CharactersDataEditManager::view() const
{
	return m_editor;
}
