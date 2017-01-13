#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <QRegExpValidator>
#include <QString>


/**
 * @brief Класс - сборник валидоторов под разные нужды
 */
class Validator
{
public:
	/**
	 * @brief Проверить валидность адреса электронной почты
	 */
	static bool isEmailValid(const QString& _email)
	{
		//
		// Для валидатора нужна неконстантная ссылка,
		// поэтому копируем
		//
		QString toCheck = _email;

		QRegExpValidator validator(QRegExp(".+@.{2,}\\..{2,}"));
		int pos = 0;
		if (validator.validate(toCheck, pos) != QValidator::Acceptable) {
			return false;
		} else {
			return true;
		}
	}
};

#endif // VALIDATORS_H
