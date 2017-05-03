#include "ResearchStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ResearchMapper.h>

#include <Domain/Research.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


ResearchTable* ResearchStorage::all()
{
    if (m_all == 0) {
        m_all = MapperFacade::researchMapper()->findAll();
    }
    return m_all;
}

Research* ResearchStorage::research(const QString& _name)
{
    Research* resultResearch = 0;
    foreach (DomainObject* domainObject, all()->toList()) {
        Research* research = dynamic_cast<Research*>(domainObject);
        if (research->name() == _name) {
            resultResearch = research;
            break;
        }
    }
    return resultResearch;
}

Research* ResearchStorage::storeResearch(Research* _parent, int _researchType, int _sortOrder,
    const QString& _researchName)
{
    //
    // Создаём новую разработку
    //
    Research* newResearch =
        new Research(Identifier(), _parent, (Research::Type)_researchType, _sortOrder, _researchName);

    //
    // И сохраняем её
    //
    // ... в базе данных
    //
    MapperFacade::researchMapper()->insert(newResearch);

    //
    // ... в списках
    //
    all()->append(newResearch);

    return newResearch;
}

void ResearchStorage::updateResearch(Research* _research)
{
    //
    // Сохраним изменение в базе данных
    //
    if (MapperFacade::researchMapper()->update(_research)) {
        //
        // Уведомим об обновлении
        //
        int indexRow = all()->toList().indexOf(_research);
        QModelIndex updateIndex = all()->index(indexRow, 0, QModelIndex());
        emit all()->dataChanged(updateIndex, updateIndex);
    }
}

void ResearchStorage::removeResearch(Research* _research)
{
    //
    // Если такая разработка есть
    //
    if (hasResearch(_research)) {
        //
        // Удаляем так же все вложенные разработки
        //
        QList<DomainObject*> full = all()->toList();
        QList<Research*> toDelete;
        toDelete.append(_research);
        //
        // проходим лист и собираем всех детей элемента
        // проходим лист и собираем всех детей его детей
        // проходим лист таким образом, пока не случится пустого прохода
        //
        int toDeleteCount = 0;
        do {
            toDeleteCount = 0;
            for (int index = 0; index < full.size(); ++index) {
                Research* nextResearch = dynamic_cast<Research*>(full.value(index));
                if (toDelete.contains(nextResearch->parent())) {
                    toDelete.append(nextResearch);
                    full.removeAt(index);
                    --index;
                    ++toDeleteCount;
                }
            }
        } while (toDeleteCount > 0);

        foreach (Research* research, toDelete) {
            //
            // ... удалим из локального списка и базы данных
            //
            all()->remove(research);
            if (characters()->contains(research)) {
                characters()->remove(research);
            } else if (locations()->contains(research)) {
                locations()->remove(research);
            }
            MapperFacade::researchMapper()->remove(research);
        }
    }
}

bool ResearchStorage::hasResearch(Research* _research)
{
    bool contains = false;
    foreach (DomainObject* domainObject, all()->toList()) {
        Research* research = dynamic_cast<Research*>(domainObject);
        if (research == _research) {
            contains = true;
            break;
        }
    }
    return contains;
}

void ResearchStorage::clear()
{
    delete m_all;
    m_all = nullptr;

    delete m_characters;
    m_characters = nullptr;

    delete m_locations;
    m_locations = nullptr;

    MapperFacade::researchMapper()->clear();
}

void ResearchStorage::refresh()
{
    MapperFacade::researchMapper()->refresh(all());
}

ResearchTable* ResearchStorage::characters()
{
    if (m_characters == 0) {
        m_characters = MapperFacade::researchMapper()->findCharacters();
    }
    return m_characters;
}

Research* ResearchStorage::character(const QString& _name)
{
    return research(_name);
}

Research* ResearchStorage::storeCharacter(const QString& _name, int _sortOrder)
{
    Research* newCharacter = 0;

    QString characterName = _name.toUpper().trimmed();

    //
    // Если персонажа можно сохранить
    //
    if (!characterName.isEmpty()) {
        //
        // Проверяем наличие данного персонажа
        //
        foreach (DomainObject* domainObject, characters()->toList()) {
            Research* character = dynamic_cast<Research*>(domainObject);
            if (character->name() == characterName) {
                newCharacter = character;
                break;
            }
        }

        //
        // Если такого персонажа ещё нет, то сохраним его
        //
        if (!DomainObject::isValid(newCharacter)) {
            //
            // ... в базе данных и полном списке разработок
            //
            const int sortOrder = _sortOrder == -1 ? characters()->size() : _sortOrder;
            newCharacter = storeResearch(nullptr, Research::Character, sortOrder, characterName);

            //
            // ... в текущем списке персонажей
            //
            characters()->append(newCharacter);
        }
    }

    return newCharacter;
}

void ResearchStorage::updateCharacter(Research* _character)
{
    _character->setName(_character->name().toUpper());
    updateResearch(_character);

    //
    // Уведомим об обновлении
    //
    int indexRow = characters()->toList().indexOf(_character);
    QModelIndex updateIndex = characters()->index(indexRow, 0, QModelIndex());
    emit characters()->dataChanged(updateIndex, updateIndex);
}

void ResearchStorage::removeCharacter(const QString& _name)
{
    if (hasCharacter(_name)) {
        removeResearch(character(_name));
    }
}

void ResearchStorage::removeCharacters(const QStringList& _names)
{
    for (const QString& name : _names) {
        removeCharacter(name);
    }
}

bool ResearchStorage::hasCharacter(const QString& _name)
{
    bool contains = false;
    foreach (DomainObject* domainObject, characters()->toList()) {
        Research* character = dynamic_cast<Research*>(domainObject);
        if (character->name() == _name.toUpper()) {
            contains = true;
            break;
        }
    }
    return contains;
}

ResearchTable* ResearchStorage::locations()
{
    if (m_locations == nullptr) {
        m_locations = MapperFacade::researchMapper()->findLocations();
    }
    return m_locations;
}

Research* ResearchStorage::location(const QString& _name)
{
    return research(_name);
}

Research* ResearchStorage::storeLocation(const QString& _name, int _sortOrder)
{
    Research* newLocation = 0;

    QString locationName = _name.toUpper().trimmed();

    //
    // Если локацию можно сохранить
    //
    if (!locationName.isEmpty()) {
        //
        // Проверяем наличие данной локации
        //
        foreach (DomainObject* domainObject, locations()->toList()) {
            Research* location = dynamic_cast<Research*>(domainObject);
            if (location->name() == locationName) {
                newLocation = location;
                break;
            }
        }

        //
        // Если такой локации ещё нет, то сохраним её
        //
        if (!DomainObject::isValid(newLocation)) {
            //
            // ... в базе данных и полном списке разработок
            //
            const int sortOrder = _sortOrder == -1 ? locations()->size() : _sortOrder;
            newLocation = storeResearch(nullptr, Research::Location, sortOrder, locationName);

            //
            // ... в текущем списке локаций
            //
            locations()->append(newLocation);
        }
    }

    return newLocation;
}

void ResearchStorage::updateLocation(Research* _location)
{
    _location->setName(_location->name().toUpper());
    updateResearch(_location);

    //
    // Уведомим об обновлении
    //
    int indexRow = locations()->toList().indexOf(_location);
    QModelIndex updateIndex = locations()->index(indexRow, 0, QModelIndex());
    emit locations()->dataChanged(updateIndex, updateIndex);
}

void ResearchStorage::removeLocation(const QString& _name)
{
    if (hasLocation(_name)) {
        removeResearch(location(_name));
    }
}

void ResearchStorage::removeLocations(const QStringList& _names)
{
    for (const QString& name : _names) {
        removeLocation(name);
    }
}

bool ResearchStorage::hasLocation(const QString& _name)
{
    bool contains = false;
    foreach (DomainObject* domainObject, locations()->toList()) {
        Research* location = dynamic_cast<Research*>(domainObject);
        if (location->name() == _name.toUpper()) {
            contains = true;
            break;
        }
    }
    return contains;
}

ResearchStorage::ResearchStorage()
{
}
