#ifndef CHARACTERSMANAGER_H
#define CHARACTERSMANAGER_H

#include <QObject>

class QSplitter;

namespace ManagementLayer
{
	class CharactersNavigatorManager;
	class CharactersDataEditManager;


	/**
	 * @brief Управляющий персонажами
	 */
	class CharactersManager : public QObject
	{
		Q_OBJECT

	public:
		explicit CharactersManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

        /**
         * @brief Очистить данные текущего проекта
         */
        void closeCurrentProject();

		/**
		 * @brief Сохранить персонажи сценария
		 */
		void saveCharacters();

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Был изменён персонаж
		 */
		void characterChanged();

		/**
		 * @brief Было изменено имя персонажа
		 */
		void characterNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Обновить список персонажей
		 */
		void refreshCharacters();

	private slots:
		/**
		 * @brief Добавить персонажа
		 */
		void aboutAddCharacter(const QString& _name);

		/**
		 * @brief Изменить персонажа
		 */
		void aboutEditCharacter(const QString& _name);

		/**
		 * @brief Удалить персонажей
		 */
		void aboutRemoveCharacters(const QStringList& _names);

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
		 * @brief Представление
		 */
		QWidget* m_view;

		/**
		 * @brief Разделитель представленя
		 */
		QSplitter* m_viewSplitter;

		/**
		 * @brief Управляющий списком персонажей
		 */
		CharactersNavigatorManager* m_navigatorManager;

		/**
		 * @brief Управляющий параметрами персонажей
		 */
		CharactersDataEditManager* m_dataEditManager;
	};
}

#endif // CHARACTERSMANAGER_H
