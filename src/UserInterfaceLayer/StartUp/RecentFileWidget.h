#ifndef RECENTFILEWIDGET_H
#define RECENTFILEWIDGET_H

#include <QWidget>

class QLabel;
class ElidedLabel;


namespace UserInterface
{
	/**
	 * @brief Виджет для отображения файла в списке недавно открытых
	 */
	class RecentFileWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit RecentFileWidget(QWidget *parent = 0);

		void setProjectName(const QString& _projectName);
		void setFilePath(const QString& _filePath);

	private:
		/**
		 * @brief Название проекта
		 */
		QLabel* m_projectName;

		/**
		 * @brief Путь к файлу проекта
		 */
		ElidedLabel* m_filePath;
	};
}

#endif // RECENTFILEWIDGET_H
