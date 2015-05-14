#ifndef SCENARIONAVIGATOR_H
#define SCENARIONAVIGATOR_H

#include <QWidget>
#include <QModelIndex>

class FlatButton;
class QAbstractItemModel;
class QLabel;
class QTreeView;

namespace UserInterface
{
	class ScenarioNavigatorItemDelegate;


	/**
	 * @brief Навигатор по сценарию
	 */
	class ScenarioNavigator : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioNavigator(QWidget *parent = 0);

		/**
		 * @brief Установить количество сцен
		 */
		void setScenesCount(int _scenesCount);

		/**
		 * @brief Установить модель навигации
		 */
		void setModel(QAbstractItemModel* _model);

		/**
		 * @brief Установить текущий элемент
		 */
		void setCurrentIndex(const QModelIndex& _index);

		/**
		 * @brief Настроить отображение номеров сцен
		 */
		void setShowSceneNumber(bool _show);

		/**
		 * @brief Настроить отображение примечания сцен
		 */
		void setShowSceneDescription(bool _show);

		/**
		 * @brief Настроить что отображать в примечании к сцене, её текст (true) или синопсис (false)
		 */
		void setSceneDescriptionIsSceneText(bool _isSceneText);

		/**
		 * @brief Настроить высоту поля примечания
		 */
		void setSceneDescriptionHeight(int _height);

		/**
		 * @brief Пересоздать делегат отображения элементов в навигаторе
		 * @note Приходится именно пересоздавать навигатор, т.к. другого рабочего способа для
		 *		 обновления делегата не нашёл. Проблемы возникают при изменении размера виджета,
		 *		 который рисует делегат
		 */
		void resetView();

		/**
		 * @brief Установить флаг, что навигатор работает с черновиком
		 */
		void setIsDraft(bool _isDraft);

		/**
		 * @brief Убрать выделение
		 */
		void clearSelection();

		/**
		 * @brief Установить видимость черновика
		 */
		void setDraftVisible(bool _visible);

		/**
		 * @brief Установить видимость заметок
		 */
		void setNoteVisible(bool _visible);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Запрос на добавление элемента
		 */
		void addItem(const QModelIndex& _itemIndex);

		/**
		 * @brief Запрос на удаление элемента
		 */
		void removeItems(const QModelIndexList& _itemIndex);

		/**
		 * @brief Показать/скрыть заметки к сцене
		 */
		void showHideDraft();

		/**
		 * @brief Показать/скрыть заметки к сцене
		 */
		void showHideNote();

		/**
		 * @brief Активирована сцена
		 */
		void sceneChoosed(const QModelIndex& _sceneIndex);

		/**
		 * @brief Запрос отмены действия
		 */
		void undoPressed();

		/**
		 * @brief Запрос повтора действия
		 */
		void redoPressed();

	protected:
		/**
		 * @brief Переопределяется чтобы отлавливать нажатия Ctrl+Z и Ctrl+Shift+Z в дереве
		 */
		bool eventFilter(QObject* _watched, QEvent* _event);

	private slots:
		/**
		 * @brief Обработка запроса на добаление элемента
		 */
		void aboutAddItem();

		/**
		 * @brief Обработка запроса на удаление элемента
		 */
		void aboutRemoveItem();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Заголовок корзины
		 */
		QLabel* m_draftTitle;

		/**
		 * @brief Префикс счётчика сцен
		 */
		QLabel* m_scenesCountTitle;

		/**
		 * @brief Количество сцен в сценарии
		 */
		QLabel* m_scenesCount;

		/**
		 * @brief Кнопка удаления локации
		 */
		FlatButton* m_addItem;

		/**
		 * @brief Кнопка обновления списка локаций
		 */
		FlatButton* m_removeItem;

		/**
		 * @brief Разделитель панели инструментов
		 */
		QLabel* m_middleTitle;

		/**
		 * @brief Кнопка отображения/сокрытия черновика
		 */
		FlatButton* m_showDraft;

		/**
		 * @brief Кнопка отображения/сокрытия примечания к сцене
		 */
		FlatButton* m_showNote;

		/**
		 * @brief Окончание панели инструментов
		 */
		QLabel* m_endTitle;

		/**
		 * @brief Дерево навигации
		 */
		QTreeView* m_navigationTree;

		/**
		 * @brief Делегат дерева
		 */
		ScenarioNavigatorItemDelegate* m_navigationTreeDelegate;
	};
}

#endif // SCENARIONAVIGATOR_H
