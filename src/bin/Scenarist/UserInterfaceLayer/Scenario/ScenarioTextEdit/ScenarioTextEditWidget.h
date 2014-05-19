#ifndef SCENARIOTEXTEDITWIDGET_H
#define SCENARIOTEXTEDITWIDGET_H

#include <QFrame>

class QComboBox;
class QLabel;
class QToolButton;

namespace BusinessLogic {
	class ScenarioTextDocument;
}

namespace UserInterface
{
	class ScenarioTextEdit;


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
		 * @brief Установить документ для редактирования
		 */
		void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document);

		/**
		 * @brief Установить хронометраж
		 */
		void setDuration(const QString& _duration);

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
		void setTextEditZoomRange(int _zoomRange);

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
		void zoomRangeChanged(int _zoomRange);

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
		 * @brief Настроить соединения
		 */
		void initConnections();

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
		QToolButton* m_undo;

		/**
		 * @brief Кнопка повтора действия
		 */
		QToolButton* m_redo;

		/**
		 * @brief Префикс хронометража
		 */
		QLabel* m_durationTitle;

		/**
		 * @brief Хронометраж сценария
		 */
		QLabel* m_duration;

		/**
		 * @brief Собственно редактор текста
		 */
		ScenarioTextEdit* m_editor;

		/**
		 * @brief MD5-хэш текста сценария, используется для отслеживания изменённости текста
		 */
		QByteArray m_lastTextMd5Hash;
	};
}

#endif // SCENARIOTEXTEDITWIDGET_H
