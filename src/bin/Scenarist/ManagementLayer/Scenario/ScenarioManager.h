#ifndef SCENARIOMANAGER_H
#define SCENARIOMANAGER_H

#include <QObject>
#include <QTimer>
#include <QModelIndex>

class FlatButton;
class QSplitter;
class QComboBox;
class QLabel;
class QStackedWidget;
class TabBar;

namespace BusinessLogic {
	class ScenarioDocument;
}

namespace ManagementLayer
{
	class ScenarioNavigatorManager;
	class ScenarioSceneSynopsisManager;
	class ScenarioDataEditManager;
	class ScenarioTextEditManager;


	/**
	 * @brief Управляющий сценарием
	 */
	class ScenarioManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Получить документ сценария
		 */
		BusinessLogic::ScenarioDocument* scenario() const;

		/**
		 * @brief Получить черновик сценария
		 */
		BusinessLogic::ScenarioDocument* scenarioDraft() const;

		/**
		 * @brief Получить название сценария
		 */
		QString scenarioName() const;

		/**
		 * @brief Получить текущую позицию курсора
		 */
		int cursorPosition() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

		/**
		 * @brief Запустить таймер сохранения изменений
		 */
		void startChangesHandling();

		/**
		 * @brief Загрузить настройки текущего проекта
		 */
		void loadCurrentProjectSettings(const QString& _projectPath);

		/**
		 * @brief Сохранить данные текущего проекта
		 */
		void saveCurrentProject();

		/**
		 * @brief Сохранить настройки текущего проекта
		 */
		void saveCurrentProjectSettings(const QString& _projectPath);

		/**
		 * @brief Загрузить состояние окна
		 */
		void loadViewState();

		/**
		 * @brief Сохранить состояние окна
		 */
		void saveViewState();

		/**
		 * @brief Закрыть текущий проект
		 */
		void closeCurrentProject();

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	public slots:
		/**
		 * @brief Обновить параметры текстового редактора
		 */
		void aboutTextEditSettingsUpdated();

		/**
		 * @brief Обновить параметры навигатора
		 */
		void aboutNavigatorSettingsUpdated();

		/**
		 * @brief Пересчитать хронометраж
		 */
		void aboutChronometrySettingsUpdated();

		/**
		 * @brief Обновить счётчики
		 */
		void aboutCountersSettingsUpdated();

		/**
		 * @brief Изменено имя персонажа
		 */
		void aboutCharacterNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Пересоздать персонажей
		 */
		void aboutRefreshCharacters();

		/**
		 * @brief Изменено название локации
		 */
		void aboutLocationNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Пересоздать локации
		 */
		void aboutRefreshLocations();

		/**
		 * @brief Изменено название сценария
		 */
		void aboutScenarioNameChanged(const QString& _name);

		/**
		 * @brief Применить патч к сценарию
		 */
		/** @{ */
		void aboutApplyPatch(const QString& _patch, bool _isDraft);
		void aboutApplyPatches(const QList<QString>& _patches, bool _isDraft);
		/** @} */

		/**
		 * @brief Получены новые позиции курсоров пользователей
		 */
		void aboutCursorsUpdated(const QMap<QString, int>& _cursors, bool _isDraft);

	signals:
		/**
		 * @brief Сценарий изменён
		 */
		void scenarioChanged();

		/**
		 * @brief Перейти в полноэкранный режим
		 */
		void showFullscreen();

		/**
		 * @brief Изменения сценария сохранены
		 */
		void scenarioChangesSaved();

		/**
		 * @brief Обновлена позиция курсора
		 * @note Используется для отправки на сервер информации о курсоре
		 */
		void cursorPositionUpdated(int _position, bool _isDraft);

	private slots:
		/**
		 * @brief Обновить хронометраж
		 */
		void aboutUpdateDuration(int _cursorPosition);

		/**
		 * @brief Обновить счётчики
		 */
		void aboutUpdateCounters();

		/**
		 * @brief Обновить синопсис, если сменилась сцена
		 */
		void aboutUpdateCurrentSynopsis(int _cursorPosition);

		/**
		 * @brief Текст синопсиса текущей сцены был изменён
		 */
		void aboutUpdateCurrentSceneSynopsis(const QString& _synopsis);

		/**
		 * @brief Собрать синопсис из сцен и установить его в редактор данных сценария
		 */
		void aboutBuildSynopsisFromScenes();

		/**
		 * @brief Выделить текущую сцену в навигаторе
		 */
		void aboutSelectItemInNavigator(int _cursorPosition);

		/**
		 * @brief Сместить курсор к выбранной сцене
		 */
		/** @{ */
		void aboutMoveCursorToItem(const QModelIndex& _index);
		void aboutMoveCursorToItem(int _itemPosition);
		/** @} */

		/**
		 * @brief Добавить элемент, после заданного
		 */
		void aboutAddItem(const QModelIndex& _afterItemIndex, int _itemType, const QString& _header,
						  const QColor& _color, const QString& _description);

		/**
		 * @brief Удалить заданныё элемент
		 */
		void aboutRemoveItems(const QModelIndexList& _itemIndex);

		/**
		 * @brief Установить цвет элемента
		 */
		void aboutSetItemColor(const QModelIndex& _itemIndex, const QColor& _color);

		/**
		 * @brief Показать/скрыть заметки к сцене
		 */
		void aboutShowHideDraft();

		/**
		 * @brief Показать/скрыть заметки к сцене
		 */
		void aboutShowHideNote();

		/**
		 * @brief Сохранить изменение текста
		 */
		void aboutSaveScenarioChanges();

	private:
		/**
		 * @brief Загрузить данные
		 */
		void initData();

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

		/**
		 * @brief Изменить текущий режим работы в зависимости от испустившего сигнал навигатора
		 */
		void setWorkingMode(QObject* _sender);

		/**
		 * @brief Получить документ сценария в соответсвии с режимом работы
		 */
		BusinessLogic::ScenarioDocument* workingScenario() const;

	private:
		/**
		 * @brief Представление сценария
		 */
		QWidget* m_view;

		/**
		 * @brief Разделители представленя
		 */
		/** @{ */
		QSplitter* m_mainViewSplitter;
		QSplitter* m_draftViewSplitter;
		QSplitter* m_noteViewSplitter;
		/** @} */

		/**
		 * @brief Вкладки редакторов
		 */
		TabBar* m_viewEditorsTabs;

		/**
		 * @brief Кнопка перехода в полноэкранный режим
		 */
		FlatButton* m_showFullscreen;

		/**
		 * @brief Панели инструментов редакторов
		 */
		QStackedWidget* m_viewEditorsToolbars;

		/**
		 * @brief Редакторы
		 */
		QStackedWidget* m_viewEditors;

		/**
		 * @brief Документ сценария
		 */
		BusinessLogic::ScenarioDocument* m_scenario;

		/**
		 * @brief Документ черновика сценария
		 */
		BusinessLogic::ScenarioDocument* m_scenarioDraft;

		/**
		 * @brief Управляющий навигацией по сценарию
		 */
		ScenarioNavigatorManager* m_navigatorManager;

		/**
		 * @brief Управляющий навигацией по черновику сценария
		 */
		ScenarioNavigatorManager* m_draftNavigatorManager;

		/**
		 * @brief Управляющий синопсисом сцены
		 */
		ScenarioSceneSynopsisManager* m_sceneSynopsisManager;

		/**
		 * @brief Управляющий редактированием информации о сценарие
		 */
		ScenarioDataEditManager* m_dataEditManager;

		/**
		 * @brief Управляющий редактированием сценария
		 */
		ScenarioTextEditManager* m_textEditManager;

		/**
		 * @brief Текущий рабочий режим
		 */
		bool m_workModeIsDraft;

		/**
		 * @brief Курсоры соавторов
		 */
		/** @{ */
		QMap<QString, int> m_cleanCursors;
		QMap<QString, int> m_draftCursors;
		/** @} */

		/**
		 * @brief Таймер для сохранения изменений сценария
		 */
		QTimer m_saveChangesTimer;
	};
}

#endif // SCENARIOMANAGER_H
