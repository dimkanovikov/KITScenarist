#ifndef KEYPRESSHANDLERFACADE_H
#define KEYPRESSHANDLERFACADE_H

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QMap>

class QKeyEvent;

namespace UserInterface {
	class ScenarioTextEdit;
}

namespace KeyProcessingLayer
{
	class AbstractKeyHandler;
	class PrepareHandler;
	class PreHandler;
	class SceneHeadingHandler;
	class SceneCharactersHandler;
	class ActionHandler;
	class CharacterHandler;
	class ParentheticalHandler;
	class DialogHandler;
	class TransitionHandler;
	class NoteHandler;
	class TitleHeaderHandler;
	class TitleHandler;
	class NoprintableTextHandler;
	class SceneGroupHeaderHandler;
	class SceneGroupFooterHandler;
	class FolderHeaderHandler;
	class FolderFooterHandler;
	class SceneDescriptionHandler;

	/**
	 * @brief Класс обработчика нажатия клавиш в текстовом редакторе
	 */
	class KeyPressHandlerFacade
	{
	public:
		/**
		 * @brief Подготовиться к обработке
		 */
		void prepare(QKeyEvent* _event);

		/**
		 * @brief Предварительная обработка
		 */
		void prepareForHandle(QKeyEvent* _event);

		/**
		 * @brief Подготовить к обработке
		 */
		void prehandle();

		/**
		 * @brief Обработка
		 */
		void handle(QKeyEvent* _event, bool _pre = false);

		/**
		 * @brief Нужно ли отправлять событие в базовый класс
		 */
		bool needSendEventToBaseClass() const;

		/**
		 * @brief Нужно ли чтобы курсор был обязательно видим пользователю
		 */
		bool needEnsureCursorVisible() const;

		/**
		 * @brief Нужно ли делать подготовку к обработке блока
		 */
		bool needPrehandle() const;

	private:
		KeyPressHandlerFacade(UserInterface::ScenarioTextEdit* _editor);

		/**
		 * @brief Получить обработчик для заданного типа
		 */
		AbstractKeyHandler* handlerFor(BusinessLogic::ScenarioBlockStyle::Type _type);

	private:
		UserInterface::ScenarioTextEdit* m_editor;

		PrepareHandler* m_prepareHandler;
		PreHandler* m_preHandler;
		SceneHeadingHandler* m_sceneHeaderHandler;
		SceneCharactersHandler* m_sceneCharactersHandler;
		ActionHandler* m_actionHandler;
		CharacterHandler* m_characterHandler;
		ParentheticalHandler* m_parentheticalHandler;
		DialogHandler* m_dialogHandler;
		TransitionHandler* m_transitionHandler;
		NoteHandler* m_noteHandler;
		TitleHeaderHandler* m_titleheaderHandler;
		TitleHandler* m_titleHandler;
		NoprintableTextHandler* m_simpleTextHandler;
		SceneGroupHeaderHandler* m_sceneGroupHeaderHandler;
		SceneGroupFooterHandler* m_sceneGroupFooterHandler;
		FolderHeaderHandler* m_folderHeaderHandler;
		FolderFooterHandler* m_folderFooterHandler;
		SceneDescriptionHandler* m_sceneDescriptionHandler;

	/**
	 * @brief Одиночка
	 */
	/** @{ */
	public:
		static KeyPressHandlerFacade* instance(UserInterface::ScenarioTextEdit* _editor);

	private:
		static QMap<UserInterface::ScenarioTextEdit*, KeyPressHandlerFacade*> s_instance;
	/** @} */
	};
}

#endif // KEYPRESSHANDLERFACADE_H
