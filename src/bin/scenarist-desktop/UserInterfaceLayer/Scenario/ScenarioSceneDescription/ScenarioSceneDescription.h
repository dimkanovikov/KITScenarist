#ifndef SCENARIOSCENESYNOPSIS_H
#define SCENARIOSCENESYNOPSIS_H

#include <QWidget>

class ElidedLabel;
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
		 * @brief Установить заголовок
		 */
		void setHeader(const QString& _header);

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
		 * @brief Текст описания изменился
		 */
		void descriptionChanged(const QString& _description);

	private slots:
		/**
		 * @brief Обработка изменения текста описания
		 */
		void aboutDescriptionChanged();

	private:
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
		ElidedLabel* m_title;

		/**
		 * @brief Редактор описания
		 */
		SimpleTextEditorWidget* m_description;

		/**
		 * @brief Исходный текст описания, используется для контроля изменений
		 */
		QString m_sourceDescription;
	};
}

#endif // SCENARIOSCENESYNOPSIS_H
