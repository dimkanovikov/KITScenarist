#ifndef SCENARIODOCUMENT_H
#define SCENARIODOCUMENT_H

#include <QObject>
#include <QMap>
#include <QUuid>

class QTextDocument;
class QAbstractItemModel;


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
		QAbstractItemModel* model() const;

		/**
		 * @brief Вспомогательная функция для определения позиции вставки майм-данных в документ
		 */
		int positionToInsertMime(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore) const;

		/**
		 * @brief Определить начальную позицию элемента в тексте
		 */
		int itemStartPosition(ScenarioModelItem* _item) const;

		/**
		 * @brief Получить последнюю позицию элемента в тексте
		 */
		int itemEndPosition(ScenarioModelItem* _item) const;

		/**
		 * @brief Посчитать длительность сценария до указанной позиции
		 */
		int durationToPosition(int _position) const;

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
		ScenarioModelItem* itemForPosition(int _position, bool _findNear = false);

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
