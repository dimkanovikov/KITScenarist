#ifndef QVARIANTMAPWRITER
#define QVARIANTMAPWRITER

#include <QDataStream>
#include <QVariant>


/**
 * @brief Вспомогательный класс для преобразования карты в массив байт и обратно
 */
class QVariantMapWriter
{
public:
	/**
	 * @brief Преобразовать карту в строку с данными
	 */
	static QString mapToDataString(const QVariantMap& _map) {
		const QVariant mapValue = _map;
		QByteArray bytes;
		QDataStream bytesWriter(&bytes, QIODevice::WriteOnly);
		bytesWriter << mapValue;
		return QString(bytes.toBase64());
	}

	/**
	 * @brief Преобразовать строку с данными в карту
	 */
	static QVariantMap dataStringToMap(const QString& _dataString) {
		QByteArray bytes = QByteArray::fromBase64(_dataString.toLatin1());
		QDataStream bytesReader(&bytes, QIODevice::ReadOnly);
		QVariant mapValue;
		bytesReader >> mapValue;
		return mapValue.toMap();
	}
};

#endif // QVARIANTMAPWRITER

