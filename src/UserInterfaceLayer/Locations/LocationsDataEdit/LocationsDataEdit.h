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

		/**
		 * @brief Установить описание локации
		 */
		void setDescription(const QString& _description);

		/**
		 * @brief Получить описание локации
		 */
		QString description() const;

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
		 * @brief Обновить состояние кнопки сохранения
		 */
		void updateSaveState();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

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

		/**
		 * @brief Исходное название
		 */
		QString m_sourceName;

		/**
		 * @brief Исходное описание
		 */
		QString m_sourceDescription;
	};
}

#endif // LOCATIONSDATAEDIT_H
