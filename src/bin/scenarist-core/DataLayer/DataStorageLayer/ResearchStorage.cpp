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
	MapperFacade::researchMapper()->update(_research);

	//
	// Уведомим об обновлении
	//
	int indexRow = all()->toList().indexOf(_research);
	QModelIndex updateIndex = all()->index(indexRow, 0, QModelIndex());
	emit all()->dataChanged(updateIndex, updateIndex);
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
	m_all = 0;

	MapperFacade::researchMapper()->clear();
}

void ResearchStorage::refresh()
{
	MapperFacade::researchMapper()->refresh(all());
}

ResearchStorage::ResearchStorage() :
	m_all(0)
{
}
