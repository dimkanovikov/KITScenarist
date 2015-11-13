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
		qreal durationAtPosition(int _position) const;

		/**
		 * @brief Длительность всего сценария
		 */
		int fullDuration() const;

		/**
		 * @brief Показания счётчиков
		 */
		QString countersInfo() const;

		/**
		 * @brief Индекс элемента дерева в указанной позиции
		 */
		QModelIndex itemIndexAtPosition(int _position) const;

		/**
		 * @brief Позиция начала сцены
		 */
		int itemStartPosition(const QModelIndex& _index) const;

		/**
		 * @brief Позиция конца сцены
		 */
		int itemEndPosition(const QModelIndex& _index) const;

		/**
		 * @brief Заголовок сцены в позиции
		 */
		QString itemHeaderAtPosition(int _position) const;

		/**
		 * @brief Цвета сцены
		 */
		QString itemColors(ScenarioModelItem* _item) const;

		/**
		 * @brief Установить цвет для сцены в указанной позиции
		 */
		void setItemColorsAtPosition(int _position, const QString& _colors);

		/**
		 * @brief Синопсис сцены в позиции
		 */
		QString itemSynopsisAtPosition(int _position) const;

		/**
		 * @brief Синопсис сцены
		 */
		QString itemSynopsis(ScenarioModelItem* _item) const;

		/**
		 * @brief Установить синопсис для сцены в указанной позиции курсора
		 */
		void setItemSynopsisAtPosition(int _position, const QString& _synopsis);

		/**
		 * @brief Построить синопсис из синопсисов сцен
		 */
		QString builSynopsisFromScenes() const;

		/**
		 * @brief Загрузить документ из сценария
		 */
		void load(Domain::Scenario* _scenario);

		/**
		 * @brief Получить сценарий из которого загружен документ
		 */
		Domain::Scenario* scenario() const;

		/**
		 * @brief Установить сценарий
		 */
		void setScenario(Domain::Scenario* _scenario);

		/**
		 * @brief Сохранить документ в строку
		 */
		QString save() const;

		/**
		 * @brief Перезагрузить сценарий
		 */
		void refresh();

		/**
		 * @brief Очистить сценарий
		 */
		void clear();

		/**
		 * @brief Найти всех персонажей сценария
		 */
		QStringList findCharacters() const;

		/**
		 * @brief Найти все локации сценария
		 */
		QStringList findLocations() const;

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
		 * @brief Отключить соединения
		 */
		void removeConnections();

		/**
		 * @brief Обновить элемент структуры из промежутка текста в который он входит
		 */
		void updateItem(ScenarioModelItem* _item, int _itemStartPos, int _itemEndPos);

		/**
		 * @brief Создать или получить существующий элемент для позиции в документе
		 *		  или ближайший к позиции
		 */
		ScenarioModelItem* itemForPosition(int _position, bool _findNear = false) const;

		/**
		 * @brief Обновить номера сцен в блоках информации документа
		 */
		void updateDocumentScenesNumbers();

		/**
		 * @brief Загрузить документ из сценария
		 */
		void load(const QString& _scenario);

	private:
		/**
		 * @brief Собственно сам сценарий
		 */
		Domain::Scenario* m_scenario;

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

		/**
		 * @brief MD5-хэш текста сценария, используется для отслеживания изменённости текста
		 */
		QByteArray m_lastTextMd5Hash;
	};
}

#endif // SCENARIODOCUMENT_H
