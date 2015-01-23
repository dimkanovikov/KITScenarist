#ifndef SCENARIOTEXTEDIT_H
#define SCENARIOTEXTEDIT_H

#include <3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.h>
#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

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
		void addScenarioBlock(BusinessLogic::ScenarioBlockStyle::Type _blockType);

		/**
		 * @brief Установить вид текущего блока
		 * @param Тип блока
		 */
		void changeScenarioBlockType(BusinessLogic::ScenarioBlockStyle::Type _blockType);

		/**
		 * @brief Применить тип блока ко всему тексту в блоке
		 * @param Тип для применения
		 */
		void applyScenarioTypeToBlockText(BusinessLogic::ScenarioBlockStyle::Type _blockType);

		/**
		 * @brief Получить вид блока в котором находится курсор
		 */
		BusinessLogic::ScenarioBlockStyle::Type scenarioBlockType() const;

		/**
		 * @brief Своя реализация установки курсора
		 */
		void setTextCursorReimpl(const QTextCursor& _cursor);

		/**
		 * @brief Получить значение флага сигнализирующего сохранять ли данные во время редактирования
		 */
		bool storeDataWhenEditing() const;

		/**
		 * @brief Установить значение флага сигнализирующего сохранять ли данные во время редактирования
		 */
		void setStoreDataWhenEditing(bool _store);

		/**
		 * @brief Показываются ли в редакторе номера сцен
		 */
		bool showSceneNumbers() const;

		/**
		 * @brief Установить значение необходимости отображать номера сцен
		 */
		void setShowSceneNumbers(bool _show);

	public slots:
		/**
		 * @brief Своя реализация проверки виден ли курсор на экране
		 */
		void ensureCursorVisibleReimpl();

	signals:
		/**
		 * @brief Сменился стиль под курсором
		 */
		void currentStyleChanged();

		/**
		 * @brief Изменён стиль блока
		 */
		void styleChanged();

	protected:
		/**
		 * @brief Нажатия многих клавиш обрабатываются вручную
		 */
		void keyPressEvent(QKeyEvent* _event);

		/**
		 * @brief Переопределяется для корректной загрузки больших документов
		 */
		void paintEvent(QPaintEvent* _event);

		/**
		 * @brief Переопределяем работу с буфером обмена для использования собственного майм типа данных
		 */
		/** @{ */
		bool canInsertFromMimeData(const QMimeData* _source) const;
		QMimeData* createMimeDataFromSelection() const;
		void insertFromMimeData(const QMimeData* _source);
		/** @} */

		/**
		 * @brief Переопределяем для возвращения указателя на экран (а то скроллинг сбивается)
		 */
		void resizeEvent(QResizeEvent* _event);

	private:
		/**
		 * @brief Очистить текущий блок от установленного в нём типа
		 */
		void cleanScenarioTypeFromBlock();

		/**
		 * @brief Применить заданный тип к текущему блоку редактора
		 * @param Тип блока
		 */
		void applyScenarioTypeToBlock(BusinessLogic::ScenarioBlockStyle::Type _blockType);

		/**
		 * @brief Применить стиль группы к блоку группе
		 * @param Тип для применения
		 */
		void applyScenarioGroupTypeToGroupBlock(BusinessLogic::ScenarioBlockStyle::Type _blockType);


		/**
		 * @brief Скорректировать введённый текст
		 *
		 * - изменить регистр текста, если это необходимо
		 * - убрать лишние пробелы
		 */
		void updateEnteredText(QKeyEvent* _event);

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
		 * @brief Документ
		 */
		BusinessLogic::ScenarioTextDocument* m_document;

		/**
		 * @brief Необходимо ли сохранять данные при вводе
		 */
		bool m_storeDataWhenEditing;

		/**
		 * @brief Отображать ли номер сцен
		 */
		bool m_showSceneNumbers;
	};
}

#endif // SCENARIOTEXTEDIT_H
