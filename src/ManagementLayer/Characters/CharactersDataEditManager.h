#ifndef CHARACTERSDATAEDITMANAGER_H
#define CHARACTERSDATAEDITMANAGER_H

#include <QObject>

namespace UserInterface {
	class CharactersDataEdit;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий данными персонажа
	 */
	class CharactersDataEditManager : public QObject
	{
		Q_OBJECT

	public:
		explicit CharactersDataEditManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

	private:
		/**
		 * @brief Редактор данных
		 */
		UserInterface::CharactersDataEdit* m_editor;
	};
}

#endif // CHARACTERSDATAEDITMANAGER_H
