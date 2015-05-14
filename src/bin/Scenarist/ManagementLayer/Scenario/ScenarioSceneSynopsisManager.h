#ifndef SCENARIOSCENESYNOPSISMANAGER_H
#define SCENARIOSCENESYNOPSISMANAGER_H

#include <QObject>

namespace UserInterface {
	class ScenarioSceneSynopsis;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий редактором синопсиса сцены
	 */
	class ScenarioSceneSynopsisManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioSceneSynopsisManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Установить заголовок редактора
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
		 * @brief Редактор синопсиса
		 */
		UserInterface::ScenarioSceneSynopsis* m_view;
	};
}

#endif // SCENARIOSCENESYNOPSISMANAGER_H
