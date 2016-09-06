#ifndef SCENEUNDOSTACK_H
#define SCENEUNDOSTACK_H
#include <QString>
#include <QList>


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
	 * @brief Можно повторить?
	 */
	bool canRedo();

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
	 * Все повторы, которые расположены за текущим состоянием, будут уничтожены.
	 */
	void addState(const QString& _data);

private:
	/**
	 * @brief Вернуть текущее состояние
	 */
	QString currentState();

private:
	/**
	 * @brief История изменений
	 */
	QList<QByteArray> m_history;

	/**
	 * @brief Текущее положение в истории изменений
	 */
	int m_historyIndex;
};

#endif // SCENEUNDOSTACK_H
