#ifndef CHARACTERPARSER_H
#define CHARACTERPARSER_H

class QString;


/**
 * @brief Парсер текста блока персонаж
 */
class CharacterParser
{
public:
	/**
	 * @brief Получить имя персонажа
	 */
	static QString name(const QString& _text);
};

#endif // CHARACTERPARSER_H
