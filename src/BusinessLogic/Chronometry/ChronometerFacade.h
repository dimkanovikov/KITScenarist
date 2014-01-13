#ifndef CHRONOMETERFACADE_H
#define CHRONOMETERFACADE_H

class QTextBlock;
class QString;
class QTextDocument;
class AbstractChronometer;


/**
 * @brief Фасад для доступа к рассчёту хронометража
 */
class ChronometerFacade
{
public:
	/**
	 * @brief Вычислить хронометраж последовательности ограниченной заданными блоками
	 */
	static int calculate(const QTextBlock& _fromBlock, const QTextBlock& _toBlock);

	/**
	 * @brief Вычислить хронометраж последовательности ограниченной заданными позициями
	 */
	static int calculate(QTextDocument* _document, int _fromCursorPosition, int _toCursorPosition);

private:
	/**
	 * @brief Получить необходимый для использования хронометр
	 */
	static AbstractChronometer* chronometer();

private:
	/**
	 * @brief Текущий хронометр
	 */
	static AbstractChronometer* s_chronometer;
};

#endif // CHRONOMETERFACADE_H
