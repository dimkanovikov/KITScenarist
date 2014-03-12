#ifndef SCENARIODOCUMENT_H
#define SCENARIODOCUMENT_H

#include <QObject>
#include <QMap>
#include <QUuid>

class QTextDocument;
class QAbstractItemModel;

namespace Domain {
	class Scenario;
}

namespace BusinessLogic
{
	class ScenarioXml;
	class ScenarioTextDocument;
	class ScenarioModel;
	class ScenarioModelItem;


	/**
	 * @brief Класс документа сценария
	 *
	 * Содержит в себе как текст документа, так и его древовидную модель, которые
	 * синхронизируются при редактировании одного из них
	 */
	class ScenarioDocument : public QObject
	{
		Q_OBJECT

	public:
		/**
		 * @brief Майм-тип данных сценариев
		 */
		static QString MIME_TYPE;

	public:
		explicit ScenarioDocument(QObject* _parent = 0);

		/**
		 * @brief Текстовый документ
		 */
		ScenarioTextDocument* document() const;

		/**
		 * @brief Модель сценария
		 */
		ScenarioModel* model() const;

		/**
		 * @brief Количество сцен в сценарии
		 */
		int scenesCount() const;

		/**
		 * @brief Посчитать длительность сценария до указанной позиции
		 */
		int durationAtPosition(int _position) const;

		/**
		 * @brief Длительность всего сценария
		 */
		int fullDuration() const;

		/**
		 * @brief Индекс элемента дерева в указанной позиции
		 */
		QModelIndex itemIndexAtPosition(int _position) const;

		/**
		 * @brief Позиция начала сцены
		 */
		int itemPositionAtIndex(const QModelIndex& _index) const;

		/**
		 * @brief Загрузить документ из сценария
		 */
		void load(const QString& _scenario);

		/**
		 * @brief Сохранить документ в строку
		 */
		QString save() const;

		/**
		 * @brief Пересчитать хронометраж
		 */
		void refreshDuration();

	public:
		/**
		 * @brief Вспомогательные функции для обработчика xml
		 *
		 * FIXME: плохо, что эти функции являются открытыми их нужно перенести в закрытую часть класса
		 */
		/** @{ */
		//! Определить позицию вставки майм-данных в документ
		int positionToInsertMime(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore) const;

		//! Определить начальную позицию элемента в тексте
		int itemStartPosition(ScenarioModelItem* _item) const;

		//! Получить последнюю позицию элемента в тексте
		int itemEndPosition(ScenarioModelItem* _item) const;
		/** @} */

	private slots:
		/**
		 * @brief Изменилось содержимое документа
		 */
		void aboutContentsChange(int _position, int _charsRemoved, int _charsAdded);

	private:
		/**
		 * @brief Настроить необходимые соединения
		 */
		void initConnections();

		/**
		 * @brief Обновить элемент структуры из промежутка текста в который он входит
		 */
		void updateItem(ScenarioModelItem* _item, int _itemStartPos, int _itemEndPos);

		/**
		 * @brief Создать или получить существующий элемент для позиции в документе
		 *		  или ближайший к позиции
		 */
		ScenarioModelItem* itemForPosition(int _position, bool _findNear = false) const;

	private:
		/**
		 * @brief Обработчик xml
		 */
		ScenarioXml* m_xmlHandler;

		/**
		 * @brief Документ сценария
		 */
		ScenarioTextDocument* m_document;

		/**
		 * @brief Дерево сценария
		 */
		ScenarioModel* m_model;

		/**
		 * @brief Карта элементов дерева сценария
		 */
		QMap<int, ScenarioModelItem*> m_modelItems;
	};
}

#endif // SCENARIODOCUMENT_H