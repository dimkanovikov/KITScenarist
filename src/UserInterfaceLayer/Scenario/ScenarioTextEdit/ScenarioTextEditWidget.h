#ifndef SCENARIOTEXTEDITWIDGET_H
#define SCENARIOTEXTEDITWIDGET_H

#include <QFrame>

class QComboBox;
class QLabel;

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
		 * @brief Установить документ для редактирования
		 */
		void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document);

		/**
		 * @brief Установить хронометраж
		 */
		void setDuration(const QString& _duration);

		/**
		 * @brief Включить/выключить проверку правописания
		 */
		void setUseSpellChecker(bool _use);

	signals:
		/**
		 * @brief Изменилась позиция курсора
		 */
		void cursorPositionChanged(int _position);

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

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Стили текста документа
		 */
		QComboBox* m_textStyles;

		/**
		 * @brief Хронометраж сценария
		 */
		QLabel* m_duration;

		/**
		 * @brief Собственно редактор текста
		 */
		ScenarioTextEdit* m_editor;

	};
}

#endif // SCENARIOTEXTEDITWIDGET_H
