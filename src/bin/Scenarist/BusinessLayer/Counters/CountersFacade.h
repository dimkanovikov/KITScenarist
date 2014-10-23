#ifndef COUNTERSFACADE_H
#define COUNTERSFACADE_H

class QString;
class QTextDocument;


namespace BusinessLogic
{
	/**
	 * @brief Фасад для доступа к рассчёту статистики
	 */
	class CountersFacade
	{
	public:
		/**
		 * @brief Расчитать исформацию для заданного документа
		 */
		static QString calculateCounters(QTextDocument* _document);

	private:
		/**
		 * @brief Посчитать количество страниц
		 */
		static QString pagesCounter(QTextDocument* _document);

		/**
		 * @brief Посчитать кол-во слов
		 */
		static QString wordsCounter(QTextDocument* _document);
	};
}

#endif // COUNTERSFACADE_H
