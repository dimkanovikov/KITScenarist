#include "CardsUndoStack.h"

namespace {
    /**
     * @brief Точка в истории, которой ещё нет
     */
    const int START_HISTORY_INDEX = -1;
}


CardsUndoStack::CardsUndoStack() :
    m_historyIndex(START_HISTORY_INDEX)
{
}

bool CardsUndoStack::canUndo()
{
    if (m_historyIndex > 0) {
        return true;
    }

    return false;
}

bool CardsUndoStack::needSyncUndo()
{
    if (canUndo()) {
        return m_historyNeedSync[m_historyIndex];
    }

    return false;
}

bool CardsUndoStack::canRedo()
{
    if (m_historyIndex < m_history.count() - 1) {
        return true;
    }

    return false;
}

bool CardsUndoStack::needSyncRedo()
{
    if (canRedo()) {
        return m_historyNeedSync[m_historyIndex + 1];
    }

    return false;
}


QString CardsUndoStack::undo()
{
    if (canUndo()) {
        --m_historyIndex;
        return currentState();
    }

    return QString::null;
}

QString CardsUndoStack::redo()
{
    if (canRedo()) {
        ++m_historyIndex;
        return currentState();
    }

    return QString::null;
}

void CardsUndoStack::clear()
{
    m_history.clear();
    m_historyNeedSync.clear();
    m_historyIndex = START_HISTORY_INDEX;
}

void CardsUndoStack::addState(const QString& _data, bool _needSync)
{
    //
    // +1 т.к. нужно сохранить текущее состояние
    //
    const int indexToRemove = m_historyIndex + 1;
    while (m_history.count() > indexToRemove) {
        m_history.removeAt(indexToRemove);
        m_historyNeedSync.removeAt(indexToRemove);
    }

    m_history << qCompress(_data.toUtf8());
    m_historyNeedSync << _needSync;

    ++m_historyIndex;
}

bool CardsUndoStack::hasChanges(const QString& _data)
{
    //
    // Сравниваем сжатые массивы, так быстрее
    //
    if (!m_history.isEmpty()) {
        return m_history[m_historyIndex] != qCompress(_data.toUtf8());
    }

    return true;
}

QString CardsUndoStack::currentState()
{
    if (!m_history.isEmpty()) {
        return qUncompress(m_history[m_historyIndex]);
    }

    return QString::null;
}

