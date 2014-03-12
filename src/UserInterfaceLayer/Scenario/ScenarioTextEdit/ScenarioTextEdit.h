#ifndef SCENARIOTEXTEDIT_H
#define SCENARIOTEXTEDIT_H

#include <3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>

namespace BusinessLogic {
	class ScenarioTextDocument;
}

class QCompleter;


namespace UserInterface
{
	/**
	 * @brief Текстовый редактор сценария
	 */
	class ScenarioTextEdit : public CompletableTextEdit
	{
		Q_OBJECT

	public:
		explicit ScenarioTextEdit(QWidget* _parent);

		/**
		 * @brief Установить документ для редактирования
		 */
		void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document);

		/**
		 * @brief Вставить новый блок
		 * @param Тип блока
		 */
		void addScenarioBlock(BusinessLogic::ScenarioTextBlockStyle::Type _blockType);

		/**
		 * @brief Установить вид текущего блока
		 * @param Тип блока
		 */
		void changeScenarioBlockType(BusinessLogic::ScenarioTextBlockStyle::Type _blockType);

		/**
		 * @brief Применить тип блока ко всему тексту в блоке
		 * @param Тип для применения
		 */
		void applyScenarioTypeToBlockText(BusinessLogic::ScenarioTextBlockStyle::Type _blockType);

		/**
		 * @brief Получить вид блока
		 */
		BusinessLogic::ScenarioTextBlockStyle::Type scenarioBlockType(const QTextBlock& _block) const;

		/**
		 * @brief Получить вид блока в котором находится курсор
		 */
		BusinessLogic::ScenarioTextBlockStyle::Type scenarioBlockType() const;

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
		void applyScenarioTypeToBlock(BusinessLogic::ScenarioTextBlockStyle::Type _blockType);

		/**
		 * @brief Применить стиль группы к блоку группе
		 * @param Тип для применения
		 */
		void applyScenarioGroupTypeToGroupBlock(BusinessLogic::ScenarioTextBlockStyle::Type _blockType);


		/**
		 * @brief Изменить регистр текста, если это необходимо
		 */
		void makeTextUpper(QKeyEvent* _event);

		/**
		 * @brief Оканчивается ли строка сокращением
		 */
		bool stringEndsWithAbbrev(const QString& _text);

	private:
		void initDocument(QTextDocument* _document);
		void initEditor();
		void initView();
		void initConnections();

	private:
		/**
		 * @brief Документ
		 */
		BusinessLogic::ScenarioTextDocument* m_document;
	};
}

#endif // SCENARIOTEXTEDIT_H
