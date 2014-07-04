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
		 * @brief Изменить указанный стиль
		 */
		void editStyle(const QString& _styleName);

	signals:

	private:
		/**
		 * @brief Диалог редактирования стиля
		 */
		UserInterface::StyleDialog* m_view;
	};
}

#endif // SETTINGSSTYLESMANAGER_H
