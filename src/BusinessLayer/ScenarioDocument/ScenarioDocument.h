#ifndef SCENARIODOCUMENT_H
#define SCENARIODOCUMENT_H

#include <QObject>
#include <QMap>

class QTextDocument;
class QAbstractItemModel;


namespace BusinessLogic
{
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

		/**
		 * Необхдимые методы:
		 * - загрузка/сохраниение документа из/в xml-текста
		 * - синхронизация модели и документа
		 */
	public:
		static QString MIME_TYPE;

	public:
		explicit ScenarioDocument(QObject* _parent = 0);

		/**
		 * @brief Текстовый документ
		 */
		QTextDocument* document() const;

		/**
		 * @brief Модель сценария
		 */
		QAbstractItemModel* model() const;

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

	private:
		/**
		 * @brief Документ сценария
		 */
		QTextDocument* m_document;

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
