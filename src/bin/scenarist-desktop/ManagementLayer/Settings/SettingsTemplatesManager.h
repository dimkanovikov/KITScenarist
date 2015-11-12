#ifndef SETTINGSTEMPLATESMANAGER_H
#define SETTINGSTEMPLATESMANAGER_H

#include <QObject>

namespace UserInterface {
	class TemplateDialog;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий библиотекой шаблонов
	 */
	class SettingsTemplatesManager : public QObject
	{
		Q_OBJECT

	public:
		explicit SettingsTemplatesManager(QObject *parent, QWidget* _parentWidget);

		/**
		 * @brief Создать новый шаблон
		 */
		bool newTemplate();

		/**
		 * @brief Изменить указанный шаблон
		 */
		bool editTemplate(const QString& _templateName);

	private:
		/**
		 * @brief Диалог редактирования шаблона
		 */
		UserInterface::TemplateDialog* m_view;
	};
}

#endif // SETTINGSTEMPLATESMANAGER_H
