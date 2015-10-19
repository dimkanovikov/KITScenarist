#ifndef SCENARIOSCENESYNOPSIS_H
#define SCENARIOSCENESYNOPSIS_H

#include <QWidget>

class ElidedLabel;
class SimpleTextEditor;


namespace UserInterface
{
	/**
	 * @brief Редактор синопсиса сцены
	 */
	class ScenarioSceneSynopsis : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioSceneSynopsis(QWidget* _parent = 0);

		/**
		 * @brief Установить заголовок
		 */
		void setHeader(const QString& _header);

		/**
		 * @brief Установить синопсис
		 */
		void setSynopsis(const QString& _synopsis);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Текст синопсиса изменился
		 */
		void synopsisChanged(const QString& _synopsis);

	private slots:
		/**
		 * @brief Обработка изменения текста синопсиса
		 */
		void aboutSynopsisChanged();

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
		 * @brief Редактор синопсиса
		 */
		SimpleTextEditor* m_synopsis;

		/**
		 * @brief Исходный текст синопсиса, используется для контроля изменений
		 */
		QString m_sourceSynopsis;
	};
}

#endif // SCENARIOSCENESYNOPSIS_H
