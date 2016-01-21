#ifndef SCENARIOTEXTCORRECTOR_H
#define SCENARIOTEXTCORRECTOR_H

class QTextBlock;
class QTextCursor;
class QTextDocument;


namespace BusinessLogic
{
	/**
	 * @brief Класс корректирующий текст сценария
	 */
	class ScenarioTextCorrector
	{
	public:
		/**
		 * @brief Удалить декорации в заданном интервале текста. Если _endPosition равен нулю, то
		 *		  удалять до конца.
		 */
		static void removeDecorations(const QTextCursor& _cursor, int _startPosition = 0, int _endPosition = 0);

		/**
		 * @brief Скорректировать документ на разрывах страниц
		 */
		static void correctScenarioText(QTextDocument* _document, int _startPosition);
	};
}

#endif // SCENARIOTEXTCORRECTOR_H
