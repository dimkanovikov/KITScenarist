#include "SynchronizationManager.h"

#include <QTimer>

using ManagementLayer::SynchronizationManager;


SynchronizationManager::SynchronizationManager(QObject* _parent) :
	QObject(_parent)
{
	initConnections();
}

bool SynchronizationManager::logIn(const QString& _login, const QString& _password)
{
	if (isLogged()) {
		logOut();
	}


}

void SynchronizationManager::setCursorPosition(int _position)
{
	if (m_cursorPosition != _position) {
		m_cursorPosition = _position;
	}
}

void SynchronizationManager::initConnections()
{
	QTimer* sendChangesTimer = new QTimer(this);
	connect(sendChangesTimer, SIGNAL(timeout()), this, SLOT(aboutSendChangesToServer()));

	//
	// Отправляем/получаем изменения, каждые SEND_CHANGES_INTERVAL мсек
	//
	const int SEND_CHANGES_INTERVAL = 1000;
	sendChangesTimer->start(SEND_CHANGES_INTERVAL);
}

