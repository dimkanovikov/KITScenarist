#ifndef RESEARCH_H
#define RESEARCH_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс разработки
	 */
	class Research : public DomainObject
	{
	public:
		/**
		 * @brief Типы разработок
		 */
		enum Type {
			Scenario,
			TitlePage,
			Synopsis,
			ResearchRoot,
			Folder,
			Text,
			Url,
			ImagesGallery,
			Image
		};

	public:
		Research(const Identifier& _id, Research* _parent, Type _type, const QString& _name,
			const QString& _description = QString::null, const QString& _url = QString::null,
			int _sortOrder = 0);

		/**
		 * @brief Получить родителя
		 */
		Research* parent() const;

		/**
		 * @brief Установить родителя
		 */
		void setParent(Research* _parent);

		/**
		 * @brief Получить тип
		 */
		Type type() const;

		/**
		 * @brief Установить тип
		 */
		void setType(Type _type);

		/**
		 * @brief Получить название
		 */
		QString name() const;

		/**
		 * @brief Установить название
		 */
		void setName(const QString& _name);

		/**
		 * @brief Получить описание
		 */
		QString description() const;

		/**
		 * @brief Установить описание
		 */
		void setDescription(const QString& _description);

		/**
		 * @brief Получить ссылку
		 */
		QString url() const;

		/**
		 * @brief Установить ссылку
		 */
		void setUrl(const QString& _url);

		/**
		 * @brief Получить позицию сортировки
		 */
		int sortOrder() const;

		/**
		 * @brief Установить позицию сортировки
		 */
		void setSortOrder(int _sortOrder);

	private:
		/**
		 * @brief Родительский элемент
		 */
		Research* m_parent;

		/**
		 * @brief Тип
		 */
		Type m_type;

		/**
		 * @brief Название
		 */
		QString m_name;

		/**
		 * @brief Описание
		 * @note Html-форматированный текст. Если в элементе хранится ссылка на интернет-ресурс,
		 *		 то в этом поле кэшируется содержимое интернет-страницы
		 */
		QString m_description;

		/**
		 * @brief Ссылка на интернет-ресурс
		 */
		QString m_url;

		/**
		 * @brief Порядок сортировки
		 */
		int m_sortOrder;
	};

	// ****

	class ResearchTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit ResearchTable(QObject* _parent = 0);

	public:
		enum Column {
			Undefined,
			Id,
			Name
		};

	public:
		int columnCount(const QModelIndex&) const;
		QVariant data(const QModelIndex& _index, int _role) const;

	private:
		Column sectionToColumn(int _section) const;
	};
}

#endif // RESEARCH_H
