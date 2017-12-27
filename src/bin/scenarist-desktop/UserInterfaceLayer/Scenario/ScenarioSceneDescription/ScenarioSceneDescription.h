#ifndef SCENARIOSCENESYNOPSIS_H
#define SCENARIOSCENESYNOPSIS_H

#include <QWidget>

class FlatButton;
class QFrame;
class QLineEdit;
class SimpleTextEditorWidget;


namespace UserInterface
{
	/**
	 * @brief Редактор описания сцены
	 */
	class ScenarioSceneDescription : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioSceneDescription(QWidget* _parent = 0);

		/**
		 * @brief Установить название
		 */
		void setTitle(const QString& _title);

		/**
		 * @brief Установить описание
		 */
		void setDescription(const QString& _description);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Изменилось название сцены
		 */
		void titleChanged(const QString& _title);

        /**
         * @brief Нажата кнопка копирования текста описания в текст сценария
         */
        void copyDescriptionToScriptPressed();

		/**
		 * @brief Текст описания изменился
		 */
		void descriptionChanged(const QString& _description);

	private slots:
		/**
		 * @brief Обработка изменения названия
		 */
		void aboutTitleChanged();

		/**
		 * @brief Обработка изменения текста описания
		 */
		void aboutDescriptionChanged();

	private:
		/**
		 * @brief Название
		 */
		QString currentTitle() const;

		/**
		 * @brief Текущее описание
		 */
		QString currentDescription() const;

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
		 * @brief Заголовок редактора
		 */
        QFrame* m_titleHeader = nullptr;

		/**
		 * @brief Название сцены
		 */
        QLineEdit* m_title = nullptr;

        /**
         * @brief Кнопка копирования описания сцены в текст сценария
         */
        FlatButton* m_copyToScript = nullptr;

		/**
		 * @brief Редактор описания
		 */
        SimpleTextEditorWidget* m_description = nullptr;

		/**
		 * @brief Исходный текст названия, используется для контроля изменений
		 */
		QString m_sourceTitle;

		/**
		 * @brief Исходный текст описания, используется для контроля изменений
		 */
		QString m_sourceDescription;

        /**
         * @brief Находимся ли в моменте изменения описания сцены
         */
        int m_inDescriptionChange = 0;
	};
}

#endif // SCENARIOSCENESYNOPSIS_H
