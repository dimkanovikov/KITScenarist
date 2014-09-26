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
		bool newStyle();

		/**
		 * @brief Изменить указанный стиль
		 */
		bool editStyle(const QString& _styleName);

	private:
		/**
		 * @brief Диалог редактирования стиля
		 */
		UserInterface::StyleDialog* m_view;
	};
}

#endif // SETTINGSSTYLESMANAGER_H
