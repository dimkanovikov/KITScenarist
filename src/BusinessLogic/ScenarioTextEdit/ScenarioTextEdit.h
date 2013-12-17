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
	ScenarioTextBlockStyle::Type scenarioBlockType(const QTextBlock& _block);

	/**
	 * @brief Получить вид блока в котором находится курсор
	 */
	ScenarioTextBlockStyle::Type scenarioBlockType();

signals:
	/**
	 * @brief Сменился стиль под курсором
	 */
	void currentStyleChanged();

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
	 * FIXME: Реализовать драг и дроп и корректные копирование/вставку
	 *
	 * @brief Переопределяется для предварительной обработки вставляемого текста
	 */
	void insertFromMimeData(const QMimeData *_source);

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
};

#endif // SCENARIOTEXTEDIT_H
