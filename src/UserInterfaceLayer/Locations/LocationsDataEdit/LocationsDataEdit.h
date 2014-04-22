#ifndef LOCATIONSDATAEDIT_H
#define LOCATIONSDATAEDIT_H

#include <QWidget>

namespace Ui {
	class LocationsDataEdit;
}


namespace UserInterface
{
	/**
	 * @brief Редактор данных локации
	 */
	class LocationsDataEdit : public QWidget
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

		/**
		 * @brief Установить фотографии
		 */
		void setPhotos(const QList<QPixmap>& _photos);

		/**
		 * @brief Получить список фотографий
		 */
		QList<QPixmap> photos() const;

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
		void updateState();

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

		/**
		 * @brief Исходные фотографии
		 */
		QList<QPixmap> m_sourcePhotos;
	};
}

#endif // LOCATIONSDATAEDIT_H
