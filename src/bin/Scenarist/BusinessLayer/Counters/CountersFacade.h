#ifndef COUNTERSFACADE_H
#define COUNTERSFACADE_H

class QString;
class QTextBlock;
class QTextDocument;


namespace BusinessLogic
{
	class Counter;

	/**
	 * @brief Фасад для доступа к рассчёту статистики
	 */
	class CountersFacade
	{
	public:
		/**
		 * @brief Рассчитать значения в заданном промежутке документа
		 */
		static Counter calculate(QTextDocument* _document, int _fromCursorPosition, int _toCursorPosition);

		/**
		 * @brief Рассчитать все значения для документа
		 */
		static Counter calculateFull(QTextDocument* _document);

		/**
		 * @brief Рассчитать все значения для блока
		 */
		static Counter calculateFull(const QTextBlock& _block);

		/**
		 * @brief Расчитать исформацию для заданного документа
		 */
		static QString countersInfo(int pageCount, const Counter& _counter);


	private:
		/**
		 * @brief Посчитать кол-во слов
		 */
		static int wordsCount(const QString& _text);

		/**
		 * @brief Посчитать кол-во символов с пробелами
		 */
		static int charactersWithSpacesCount(const QString& _text);

		/**
		 * @brief Посчитать кол-во символов без пробелов
		 */
		static int charactersWithoutSpacesCount(const QString& _text);

		/**
		 * @brief Посчитать количество страниц
		 */
		static QString pageInfo(int _count);

		/**
		 * @brief Посчитать кол-во слов
		 */
		static QString wordsInfo(int _count);

		/**
		 * @brief Посчитать кол-во символов
		 */
		static QString charactersInfo(int _countWithSpaces, int _countWithoutSpaces);
	};
}

#endif // COUNTERSFACADE_H
