#ifndef DATABASEHELPER
#define DATABASEHELPER

#include <QByteArray>
#include <QString>

namespace {
	/**
	 * @brief Сила сжатия
	 */
	const int COMPRESSION_LEVEL = 6;
}


namespace DatabaseLayer {
	/**
	 * @brief Вспомогательный класс для сжимания/разжимания данных
	 */
	class DatabaseHelper {
	public:
		/**
		 * @brief Сжать данные
		 */
		static QString compress(const QString& _data) {
			return qCompress(_data.toUtf8(), COMPRESSION_LEVEL).toBase64();
		}

		/**
		 * @brief Разжать данные
		 * @note В случае, если данные повреждены, возвращается исходная строка
		 */
		static QString uncompress(const QString& _data) {
			QString result = qUncompress(QByteArray::fromBase64(_data.toUtf8()));
			if (result.isEmpty()) {
				result = _data;
			}
			return result;
		}
	};
}

#endif // DATABASEHELPER

