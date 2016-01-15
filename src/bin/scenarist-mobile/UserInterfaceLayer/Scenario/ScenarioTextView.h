#ifndef SCENARIOTEXTVIEW_H
#define SCENARIOTEXTVIEW_H

#include <QWidget>
#include <QList>
#include <QPair>

class ScalableWrapper;

namespace Ui {
	class ScenarioTextView;
}

namespace BusinessLogic {
	class ScenarioTextDocument;
}


namespace UserInterface
{
	class ScenarioTextEdit;


	/**
	 * @brief Представление редактора текста сценария
	 */
	class ScenarioTextView : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioTextView(QWidget *parent = 0);
		~ScenarioTextView();

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Задать название сценария
		 */
		void setScenarioName(const QString& _name);

		/**
		 * @brief Получить текущий редактируемый документ
		 */
		BusinessLogic::ScenarioTextDocument* scenarioDocument() const;

		/**
		 * @brief Установить документ для редактирования
		 */
		void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document, bool _isDraft = false);

//		/**
//		 * @brief Установить хронометраж
//		 */
//		void setDuration(const QString& _duration);

//		/**
//		 * @brief Установить значения счётчиков
//		 */
//		void setCountersInfo(const QString& _counters);

		/**
		 * @brief Включить/выключить отображение номеров сцен
		 */
		void setShowScenesNumbers(bool _show);

		/**
		 * @brief Установить значение необходимости подсвечивать текущую строку
		 */
		void setHighlightCurrentLine(bool _highlight);

		/**
		 * @brief Установить необходимость автоматических замен
		 */
		void setAutoReplacing(bool _replacing);

		/**
		 * @brief Включить/выключить постраничное отображение
		 */
		void setUsePageView(bool _use);

		/**
		 * @brief Включить/выключить проверку правописания
		 */
		void setUseSpellChecker(bool _use);

		/**
		 * @brief Установить язык проверки орфографии
		 */
		void setSpellCheckLanguage(int _language);

		/**
		 * @brief Настроить цвета текстового редактора
		 */
		void setTextEditColors(const QColor& _textColor, const QColor& _backgroundColor);

		/**
		 * @brief Настроить масштабирование редактора сценария
		 */
		void setTextEditZoomRange(qreal _zoomRange);

		/**
		 * @brief Получить текущую позицию курсора
		 */
		int cursorPosition() const;

		/**
		 * @brief Установить позицию курсора
		 */
		void setCursorPosition(int _position);

		/**
		 * @brief Добавить элемент в указанной позиции с заданным текстом и типом
		 */
		void addItem(int _position, int _type, const QString& _header,
			const QColor& _color, const QString& _description);

		/**
		 * @brief Удалить текст в заданном интервале
		 */
		void removeText(int _from, int _to);

		/**
		 * @brief Обновить все элементы связанные с перечнем стилей блоков стиля сценария
		 */
		void updateStylesElements();

		/**
		 * @brief Обновить горячие клавиши смены блоков
		 */
		void updateShortcuts();

		/**
		 * @brief Установить список дополнительных курсоров для отрисовки
		 */
		void setAdditionalCursors(const QMap<QString, int>& _cursors);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	public slots:
		/**
		 * @brief Отменить последнее действие
		 */
		void aboutUndo();

		/**
		 * @brief Повторить последнее действие
		 */
		void aboutRedo();

	signals:
		/**
		 * @brief Нажата кнопка показать навигатор
		 */
		void showNavigatorClicked();

		/**
		 * @brief Изменился текст сценария
		 */
		void textChanged();

		/**
		 * @brief Изменилась позиция курсора
		 */
		void cursorPositionChanged(int _position);

		/**
		 * @brief Изменился коэффициент масштабирования текстового редактора
		 */
		void zoomRangeChanged(qreal _zoomRange);

	protected:
		/**
		 * @brief Переопределяем для того, чтобы обновить панель стилей блоков при повороте экрана
		 */
        void resizeEvent(QResizeEvent* _event);

	private slots:
		/**
		 * @brief Обновить текущий стиль текста
		 */
		void aboutUpdateTextStyle();

		/**
		 * @brief Сменить стиль текста
		 */
		void aboutChangeTextStyle();

		/**
		 * @brief Обработчик изменения позиции курсора
		 */
		void aboutCursorPositionChanged();

		/**
		 * @brief Обработка изменения текста
		 */
		void aboutTextChanged();

		/**
		 * @brief Обработка изменения стиля текста
		 */
		void aboutStyleChanged();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить выпадающий список стилей в зависимости от текущего стиля
		 */
		void initStylesCombo();

		/**
		 * @brief Настроить соединения для формы
		 */
		/** @{ */
		void initConnections();
		void initEditorConnections();
		void removeEditorConnections();
		/** @} */

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Интерфейс представления
		 */
		Ui::ScenarioTextView *m_ui;

		/**
		 * @brief Собственно редактор текста
		 */
		ScenarioTextEdit* m_editor;

		/**
		 * @brief Обёртка редактора, позволяющая его масштабировать
		 */
		ScalableWrapper* m_editorWrapper;

		/**
		 * @brief MD5-хэш текста сценария, используется для отслеживания изменённости текста
		 */
		QByteArray m_lastTextMd5Hash;

		/**
		 * @brief Список стилей блоков
		 */
		QList<QPair<QString, int> > m_textTypes;
	};
}

#endif // SCENARIOTEXTVIEW_H
