#ifndef SCENARIOTEXTEDIT_H
#define SCENARIOTEXTEDIT_H

#include <UserInterface/Widgets/CompletableTextEdit/CompletableTextEdit.h>
#include "ScenarioTextBlock/ScenarioTextBlockStyle.h"

class QCompleter;


class ScenarioTextEdit : public CompletableTextEdit
{
	Q_OBJECT


public slots:
	void test();

public:
	explicit ScenarioTextEdit(QWidget* _parent = 0);

	/**
	 * @brief Вставить новый блок
	 * @param Тип блока
	 */
	void addScenarioBlock(ScenarioTextBlockStyle::Type _blockType);

	/**
	 * @brief Установить вид текущего блока
	 * @param Тип блока
	 */
	void changeScenarioBlockType(ScenarioTextBlockStyle::Type _blockType);

	/**
	 * @brief Применить тип блока ко всему тексту в блоке
	 * @param Тип для применения
	 */
	void applyScenarioTypeToBlockText(ScenarioTextBlockStyle::Type _blockType);

	/**
	 * @brief Получить вид блока
	 */
	ScenarioTextBlockStyle::Type scenarioBlockType(const QTextBlock& _block) const;

	/**
	 * @brief Получить вид блока в котором находится курсор
	 */
	ScenarioTextBlockStyle::Type scenarioBlockType() const;

	/**
	 * @brief Находится ли редактор в состоянии обновления текста
	 */
	bool textUpdateInProgress() const;

	/**
	 * @brief Установить состояние обновления текста редактора
	 */
	void setTextUpdateInProgress(bool _inProgress);

signals:
	/**
	 * @brief Сменился стиль под курсором
	 */
	void currentStyleChanged();

	/**
	 * @brief Изменилась структура документа
	 */
	void structureChanged();

protected:
	/**
	 * @brief Нажатия многих клавиш обрабатываются вручную
	 */
	void keyPressEvent(QKeyEvent* _event);

	/**
	 * @brief Переопределяется для реализации увеличения/уменьшения текста
	 */
	void wheelEvent(QWheelEvent* _event);

	/**
	 * @brief Переопределяем работу с буфером обмена для использования собственного майм типа данных
	 */
	/** @{ */
	void dropEvent(QDropEvent* _event);
	bool canInsertFromMimeData(const QMimeData* _source) const;
	QMimeData* createMimeDataFromSelection() const;
	void insertFromMimeData(const QMimeData* _source);
	/** @} */

private:
	/**
	 * @brief Очистить текущий блок от установленного в нём типа
	 */
	void cleanScenarioTypeFromBlock();

	/**
	 * @brief Применить заданный тип к текущему блоку редактора
	 * @param Тип блока
	 */
	void applyScenarioTypeToBlock(ScenarioTextBlockStyle::Type _blockType);


	/**
	 * @brief Изменить регистр текста, если это необходимо
	 */
	void makeTextUpper(QKeyEvent* _event);

	/**
	 * @brief Оканчивается ли строка сокращением
	 */
	bool stringEndsWithAbbrev(const QString& _text);

private:
	void initEditor();
	void initView();
	void initConnections();

private:
	/**
	 * @brief Выполняется ли обновление текста в данный момент,
	 *		  например вставка из буфера или загрузка текста из файла
	 */
	bool m_textUpdateInProgress;
};

#endif // SCENARIOTEXTEDIT_H
