#ifndef SCENEUNDOSTACK_H
#define SCENEUNDOSTACK_H

#include <QString>
#include <QVector>


/**
 * @class SceneUndoStack
 * Хранит состояния сцены, представленные в виде XML-строк.
 * Грубо, зато быстро
 * При добавлении в стэк строка архивируется, а при извлечении разархивируется
 *
 * @see CustomGraphicsScene
 * @see CustomGraphicsScene::stateChangedByUser()
 */
class SceneUndoStack
{
public:
	SceneUndoStack();

	/**
	 * @brief Можно отменить?
	 */
	bool canUndo();

	/**
	 * @brief Нужно ли синхронизировать отмену с текстом сценария
	 */
	bool needSyncUndo();

	/**
	 * @brief Можно повторить?
	 */
	bool canRedo();

	/**
	 * @brief Нужно ли синхронизировать повтор с текстом сценария
	 */
	bool needSyncRedo();

	/**
	 * @brief Отменить (перейти к предыдущему состоянию)
	 */
	QString undo();

	/**
	 * @brief Повторить (перейти к следующему состоянию)
	 */
	QString redo();

	/**
	 * @brief Очистить историю
	 */
	void clear();

	/**
	 * @brief Добавить изменение в историю
	 * Состояние представляет собой XML-строку, полученную в CustomGraphicsScene::toXML().
	 */
	void addState(const QString& _data, bool _needSync);

	/**
	 * @brief Изменилась ли схема по сравнению с текущим состоянием
	 */
	bool hasChanges(const QString& _data);

private:
	/**
	 * @brief Вернуть текущее состояние
	 */
	QString currentState();

private:
	/**
	 * @brief История изменений
	 */
	QVector<QByteArray> m_history;

	/**
	 * @brief Нужно ли синхронизировать изменения с текстом сценария
	 */
	QVector<bool> m_historyNeedSync;

	/**
	 * @brief Текущее положение в истории изменений
	 */
	int m_historyIndex;
};

#endif // SCENEUNDOSTACK_H
