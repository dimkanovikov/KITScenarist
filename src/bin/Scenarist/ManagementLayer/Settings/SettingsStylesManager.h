#ifndef SETTINGSSTYLESMANAGER_H
#define SETTINGSSTYLESMANAGER_H

#include <QObject>

namespace UserInterface {
	class StyleDialog;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий библиотекой стилей
	 */
	class SettingsStylesManager : public QObject
	{
		Q_OBJECT

	public:
		explicit SettingsStylesManager(QObject *parent, QWidget* _parentWidget);

		/**
		 * @brief Создать новый стиль
		 */
		void newStyle();



		/**
		 * @brief Получить список стилей установленных в системе
		 */
		QMap<QString, QString> stylesList() const;

		/**
		 * @brief Установить стиль
		 */
		void setupStyle(const QString& _styleXml);

		/**
		 * @brief Получить xml-описание стиля
		 */
		QString styleXml(const QString& _styleName) const;

	signals:

	private:
		/**
		 * @brief Диалог редактирования стиля
		 */
		UserInterface::StyleDialog* m_view;
	};
}

#endif // SETTINGSSTYLESMANAGER_H
