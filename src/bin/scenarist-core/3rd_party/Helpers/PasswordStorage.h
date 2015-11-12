#ifndef PASSWORDSTORAGE_H
#define PASSWORDSTORAGE_H

#include <QString>

namespace {
	/**
	 * @brief Ключ для сохранения по умолчанию
	 */
	const QString DEFAULT_KEY = "kc+2kpY8BI6UsKDFLP4oavfK8v2Yd9hjpP46yA1C94krG1OclkT-hHvQk1tZcg";
}


/**
 * @brief Вспомогательный класс для хранения/загрузки паролей
 */
class PasswordStorage
{
public:
	/**
	 * @brief Сохранение пароля
	 */
	static QString save(const QString& _password, QString _key = QString::null)
	{
		// Перевод строк в битовые массивы с использованием локальных таблиц
		QByteArray passwordData = _password.toLocal8Bit();
		QByteArray _keyData = _key.isNull() ? DEFAULT_KEY.toLocal8Bit() : _key.toLocal8Bit();

		QString result = "";
		// Кодирование информации
		for (int i = 0; i < passwordData.size(); ++i)
		{
			for (int j = 0; j < _keyData.size(); ++j)
			{
				// XOR - кодировка символа
				passwordData[i] = passwordData[i] ^ (_keyData[j] + (i*j));
			}

			//Преобразование числа в шестнадцатеричную систему
			result += QString("%1").arg((int)((unsigned char)passwordData[i]), 2, 16, QChar('0'));
		}

		//Возврат кодированной строки
		return result;
	}

	/**
	 * @brief Загрузка пароля
	 */
	static QString load(const QString& _password, const QString& _key = QString::null)
	{
		// Декодировка строки из 16-ричной системы в битовый массив
		QByteArray passwordData = QByteArray::fromHex(_password.toLocal8Bit());
		// Перевод строки пароля в битовый массив
		QByteArray keyData = _key.isNull() ? DEFAULT_KEY.toLocal8Bit() : _key.toLocal8Bit();

		// Декодирование информации
		for (int i = 0; i < passwordData.size(); ++i)
		{
			for (int j = 0; j < keyData.size(); ++j)
			{
				// XOR - кодировка символа
				passwordData[i] = passwordData[i] ^ (keyData[j] + (i*j));
			}
		}

		//Возврат кодированной строки
		return QString::fromLocal8Bit(passwordData.data());
	}
};

#endif // PASSWORDSTORAGE_H
