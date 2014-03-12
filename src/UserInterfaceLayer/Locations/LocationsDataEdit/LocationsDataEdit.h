#ifndef LOCATIONSDATAEDIT_H
#define LOCATIONSDATAEDIT_H

#include <QDialog>

namespace Ui {
	class LocationsDataEdit;
}


namespace UserInterface
{
	/**
	 * @brief Редактор данных локации
	 */
	class LocationsDataEdit : public QDialog
	{
		Q_OBJECT

	public:
		explicit LocationsDataEdit(QWidget *parent = 0);
		~LocationsDataEdit();

		/**
		 * @brief Очистить все поля
		 */
		void clean();

		/**
		 * @brief Установить название локации
		 */
		void setName(const QString& _name);

		/**
		 * @brief Получить название локации
		 */
		QString name() const;

	signals:
		/**
		 * @brief Сохранить
		 */
		void saveLocation();

		/**
		 * @brief Перезагрузить
		 */
		void reloadLocation();

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
		Ui::LocationsDataEdit *ui;
	};
}

#endif // LOCATIONSDATAEDIT_H
