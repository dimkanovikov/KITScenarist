#include "sceneundostack.h"


SceneUndoStack::SceneUndoStack() :
	m_historyIndex(0)
{
}

bool SceneUndoStack::canUndo()
{
	if (m_historyIndex > 0 && !m_history.isEmpty()) {
		return true;
	}

	return false;
}

bool SceneUndoStack::canRedo()
{
	if (m_historyIndex < m_history.count() - 1) {
		return true;
	}

	return false;
}


QString SceneUndoStack::undo()
{
	if (canUndo()) {
		--m_historyIndex;
		return currentState();
	}

	return QString::null;
}

QString SceneUndoStack::redo()
{
	if (canRedo()) {
		++m_historyIndex;
		return currentState();
	}

	return QString::null;
}

void SceneUndoStack::clear()
{
	m_history.clear();
	m_historyIndex = 0;
}

void SceneUndoStack::addState (const QString& _data)
{
	while (m_history.count() > m_historyIndex) {
		m_history.removeAt(m_historyIndex);
	}
	m_history << qCompress(_data.toUtf8());
	++m_historyIndex;
}

QString SceneUndoStack::currentState()
{
	return qUncompress(m_history[m_historyIndex]);
}
