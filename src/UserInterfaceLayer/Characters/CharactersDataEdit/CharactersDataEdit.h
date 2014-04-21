#ifndef CHARACTERSDATAEDIT_H
#define CHARACTERSDATAEDIT_H

#include <QWidget>

namespace Ui {
	class CharactersDataEdit;
}


namespace UserInterface
{
	/**
	 * @brief Редактор данных персонажа
	 */
	class CharactersDataEdit : public QWidget
	{
		Q_OBJECT

	public:
		explicit CharactersDataEdit(QWidget *parent = 0);
		~CharactersDataEdit();

		/**
		 * @brief Очистить все поля
		 */
		void clean();

		/**
		 * @brief Установить имя персонажа
		 */
		void setName(const QString& _name);

		/**
		 * @brief Получить имя персонажа
		 */
		QString name() const;

	signals:
		/**
		 * @brief Сохранить
		 */
		void saveCharacter();

		/**
		 * @brief Перезагрузить
		 */
		void reloadCharacter();

	private slots:
		/**
		 * @brief Данные были изменены
		 */
		void aboutDataChanged();
		/**
		 * @brief Настроить представление
		 */
		void initView();
		
	private:
		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		Ui::CharactersDataEdit *ui;
	};
}

#endif // CHARACTERSDATAEDIT_H
