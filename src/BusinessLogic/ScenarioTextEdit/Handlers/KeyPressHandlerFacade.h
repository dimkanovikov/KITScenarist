#ifndef KEYPRESSHANDLERFACADE_H
#define KEYPRESSHANDLERFACADE_H

#include "../ScenarioTextBlock/ScenarioTextBlockStyle.h"

class ScenarioTextEdit;
class QKeyEvent;


namespace KeyProcessingLayer
{
	class AbstractKeyHandler;
	class PrepareHandler;
	class PreHandler;
	class TimeAndPlaceHandler;
	class ActionHandler;
	class CharacterHandler;
	class ParentheticalHandler;
	class DialogHandler;
	class TransitionHandler;
	class NoteHandler;
	class TitleHeaderHandler;
	class TitleHandler;
	class FolderHeaderHandler;
	class FolderFooterHandler;
	class SimpleTextHandler;

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
		void prehandle(QKeyEvent* _event);

		/**
		 * @brief Обработка
		 */
		void handle(QKeyEvent* _event);

		/**
		 * @brief Нужно ли отправлять событие в базовый класс
		 */
		bool needSendEventToBaseClass() const;

		/**
		 * @brief Нужно ли чтобы курсор был обязательно видим пользователю
		 */
		bool needEnshureCursorVisible() const;

		/**
		 * @brief Изменилась ли структура сценария
		 */
		bool structureChanged() const;

	private:
		KeyPressHandlerFacade(ScenarioTextEdit* _editor);

		/**
		 * @brief Получить обработчик для заданного типа
		 */
		AbstractKeyHandler* handlerFor(ScenarioTextBlockStyle::Type _type);

	private:
		ScenarioTextEdit* m_editor;

		PrepareHandler* m_prepareHandler;
		PreHandler* m_preHandler;
		TimeAndPlaceHandler* m_sceneHeaderHandler;
		ActionHandler* m_actionHandler;
		CharacterHandler* m_characterHandler;
		ParentheticalHandler* m_parentheticalHandler;
		DialogHandler* m_dialogHandler;
		TransitionHandler* m_transitionHandler;
		NoteHandler* m_noteHandler;
		TitleHeaderHandler* m_titleheaderHandler;
		TitleHandler* m_titleHandler;
		SimpleTextHandler* m_simpleTextHandler;
		FolderHeaderHandler* m_sceneGroupHeaderHandler;
		FolderFooterHandler* m_sceneGroupFooterHandler;

	/**
	 * @brief Одиночка
	 */
	/** @{ */
	public:
		static KeyPressHandlerFacade* instance(ScenarioTextEdit* _editor);

	private:
		static KeyPressHandlerFacade* s_instance;
	/** @} */
	};
}

#endif // KEYPRESSHANDLERFACADE_H
