#ifndef SCENARIOTEXTEDITWIDGET_H
#define SCENARIOTEXTEDITWIDGET_H

#include <QFrame>

class FlatButton;
class QComboBox;
class QLabel;
class ScalableWrapper;
class SearchWidget;

namespace BusinessLogic {
	class ScenarioTextDocument;
}

namespace UserInterface
{
	class ScenarioTextEdit;
	class ScenarioFastFormatWidget;


	/**
	 * @brief Виджет расширенного редактора текста сценария
	 */
	class ScenarioTextEditWidget : public QFrame
	{
		Q_OBJECT

	public:
		explicit ScenarioTextEditWidget(QWidget* _parent = 0);

		/**
		 * @brief Получить панель инструментов
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Получить текущий редактируемый документ
		 */
		BusinessLogic::ScenarioTextDocument* scenarioDocument() const;

		/**
		 * @brief Установить документ для редактирования
		 */
		void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document, bool _isDraft = false);

		/**
		 * @brief Установить хронометраж
		 */
		void setDuration(const QString& _duration);

		/**
		 * @brief Установить значения счётчиков
		 */
		void setCountersInfo(const QString& _counters);

		/**
		 * @brief Включить/выключить отображение номеров сцен
		 */
		void setShowScenesNumbers(bool _show);

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
		void addItem(int _position, const QString& _text, int _type);

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

	public slots:
		/**
		 * @brief Отменить последнее действие
		 */
		void aboutUndo();

		/**
		 * @brief Повторить последнее действие
		 */
		void aboutRedo();

		/**
		 * @brief Показать/скрыть поле поиска
		 */
		void aboutShowSearch();

		/**
		 * @brief Показать/скрыть виджет быстрого форматирования
		 */
		void aboutShowFastFormat();

	signals:
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
		void updateStylesCombo();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();
		void initEditorConnections();
		void removeEditorConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Панель инструментов
		 */
		QWidget* m_toolbar;

		/**
		 * @brief Стили текста документа
		 */
		QComboBox* m_textStyles;

		/**
		 * @brief Кнопка отмены действия
		 */
		FlatButton* m_undo;

		/**
		 * @brief Кнопка повтора действия
		 */
		FlatButton* m_redo;

		/**
		 * @brief Кнопка панели поиска
		 */
		FlatButton* m_search;

		/**
		 * @brief Кнопка панели быстрого форматирования
		 */
		FlatButton* m_fastFormat;

		/**
		 * @brief Хронометраж сценария
		 */
		QLabel* m_duration;

		/**
		 * @brief Счётчики сценария
		 */
		QLabel* m_countersInfo;

		/**
		 * @brief Собственно редактор текста
		 */
		ScenarioTextEdit* m_editor;

		/**
		 * @brief Обёртка редактора, позволяющая его масштабировать
		 */
		ScalableWrapper* m_editorWrapper;

		/**
		 * @brief Виджет поиска
		 */
		SearchWidget* m_searchLine;

		/**
		 * @brief Виджет быстрого форматирования текста
		 */
		ScenarioFastFormatWidget* m_fastFormatWidget;

		/**
		 * @brief MD5-хэш текста сценария, используется для отслеживания изменённости текста
		 */
		QByteArray m_lastTextMd5Hash;
	};
}

#endif // SCENARIOTEXTEDITWIDGET_H
