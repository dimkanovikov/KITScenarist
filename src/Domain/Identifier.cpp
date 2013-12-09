#include "Identifier.h"

using namespace Domain;

const int INVALID_VALUE = -1;

Identifier::Identifier() :
    m_id( INVALID_VALUE ),
    m_version( INVALID_VALUE ),
    m_isValid( false )
{

}

Identifier::Identifier( int id, int version ) :
    m_id( id ),
    m_version( version ),
    m_isValid( false )
{
    if ( id != INVALID_VALUE && version != INVALID_VALUE )
        m_isValid = true;
}

Identifier::~Identifier()
{

}

// Получение следующего идентификатора
Identifier Identifier::next() const
{
    return Identifier( value() + 1 );
}

// Идентификатор следующей версии
Identifier Identifier::nextVersion() const
{
    return Identifier( value(), version() + 1 );
}

// Проверка корректности
bool Identifier::isValid() const
{
    return m_isValid;
}

// Получение значения
int Identifier::value() const
{
    return m_id;
}

int Identifier::version() const
{
    return m_version;
}
