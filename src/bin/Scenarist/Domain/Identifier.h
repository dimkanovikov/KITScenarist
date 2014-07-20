#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <QString>
#include <QHash>
#include <QVariant>


namespace Domain
{
	class Identifier
	{
	public:
		Identifier();
		Identifier( int id, int version = 0 );
		virtual ~Identifier();

	public:
		Identifier next() const;
		Identifier nextVersion() const;

	public:
		bool isValid() const;
		int value() const;
		int version() const;

	private:
		int m_id;
		int m_version;
		bool m_isValid;
	};

	// ****
	// Функции необходимые для возможности использования класса в контейнерах QMap и QHash

	inline bool operator ==( const Identifier &id1, const Identifier &id2 )
	{
		return id1.value() == id2.value()
				&& id1.version() == id2.version();
	}
	inline bool operator !=( const Identifier &id1, const Identifier &id2 )
	{
		return !( id1 == id2);
	}
	inline bool operator>( const Identifier &id1, const Identifier &id2 )
	{
		if ( id1.value() != id2.value() ) {
			return id1.value() > id2.value();
		} else {
			return id1.version() > id2.version();
		}
	}
	inline bool operator<( const Identifier &id1, const Identifier &id2 )
	{
		if ( id1.value() != id2.value() ) {
			return id1.value() < id2.value();
		} else {
			return id1.version() < id2.version();
		}
	}
	inline uint qHash( const Identifier &key )
	{
		double hash = qHash( QString( "%1.%2" ).arg( key.value() ).arg( key.version() ) );
		return hash;
	}
}

Q_DECLARE_METATYPE(Domain::Identifier)

#endif // IDENTIFIER_H
