#include "ScenarioModelItem.h"

#include <QPainter>

using namespace BusinessLogic;


namespace {
	const int MAX_TEXT_LENGTH = 300;
}

ScenarioModelItem::ScenarioModelItem(int _position) :
	m_position(_position),
	m_sceneNumber(0),
	m_duration(0),
	m_type(Scene),
	m_hasNote(false),
	m_parent(0)
{
}

ScenarioModelItem::~ScenarioModelItem()
{
	qDeleteAll(m_children);
}

int ScenarioModelItem::position() const
{
	return m_position;
}

void ScenarioModelItem::setPosition(int _position)
{
	if (m_position != _position) {
		m_position = _position;
	}
}

int ScenarioModelItem::sceneNumber() const
{
	return m_sceneNumber;
}

void ScenarioModelItem::setSceneNumber(int _number)
{
	if (m_sceneNumber != _number) {
		m_sceneNumber = _number;
	}
}

QString ScenarioModelItem::header() const
{
	return m_header;
}

void ScenarioModelItem::setHeader(const QString& _header)
{
	if (m_header != _header) {
		m_header = _header;
		updateParentText();
	}
}

QString ScenarioModelItem::colors() const
{
	return m_colors;
}

void ScenarioModelItem::setColors(const QString& _colors)
{
	if (m_colors != _colors) {
		m_colors = _colors;
	}
}

QString ScenarioModelItem::description() const
{
	return m_description;
}

void ScenarioModelItem::setDescription(const QString& _description)
{
	if (m_description != _description) {
		m_description = _description;
	}
}

QString ScenarioModelItem::text() const
{
	return m_text;
}

void ScenarioModelItem::setText(const QString& _text)
{
	const QString newText = _text.left(MAX_TEXT_LENGTH);
	if (m_text != newText) {
		m_text = newText;
		updateParentText();
	}
}

qreal ScenarioModelItem::duration() const
{
	return m_duration;
}

void ScenarioModelItem::setDuration(qreal _duration)
{
	if (m_duration != _duration) {
		m_duration = _duration;
		updateParentDuration();
	}
}

ScenarioModelItem::Type ScenarioModelItem::type() const
{
	return m_type;
}

void ScenarioModelItem::setType(ScenarioModelItem::Type _type)
{
	if (m_type != _type) {
		m_type = _type;
	}
}

QPixmap ScenarioModelItem::icon() const
{
	QString iconPath;

	switch (m_type) {
		case Scene: {
			if (!hasNote()) {
				iconPath = ":/Graphics/Icons/scene.png";
			} else {
				iconPath = ":/Graphics/Icons/scene_noted.png";
			}
			break;
		}

		case SceneGroup: {
			if (!hasNote()) {
				iconPath = ":/Graphics/Icons/scene_group.png";
			} else {
				iconPath = ":/Graphics/Icons/scene_group_noted.png";
			}
			break;
		}

		case Folder: {
			if (!hasNote()) {
				iconPath = ":/Graphics/Icons/folder.png";
			} else {
				iconPath = ":/Graphics/Icons/folder_noted.png";
			}
			break;
		}

		case Scenario: {
			iconPath = ":/Graphics/Icons/script.png";
			break;
		}

		default: {
			iconPath = ":/Graphics/Icons/unknown.png";
			break;
		}
	}

	return QPixmap(iconPath);
}

bool ScenarioModelItem::hasNote() const
{
	return m_hasNote;
}

void ScenarioModelItem::setHasNote(bool _hasNote)
{
	if (m_hasNote != _hasNote) {
		m_hasNote = _hasNote;
	}
}

Counter ScenarioModelItem::counter() const
{
	return m_counter;
}

void ScenarioModelItem::setCounter(const Counter& _counter)
{
	if (m_counter != _counter) {
		m_counter = _counter;
		updateParentCounter();
	}
}

void ScenarioModelItem::updateParentText()
{
	//
	// Если есть дети - обновляем свой текст
	//
	if (hasChildren()) {
		QString text;
		foreach (ScenarioModelItem* child, m_children) {
			text += child->header() + " " + child->text() + " ";
			if (text.length() > MAX_TEXT_LENGTH) {
				text = text.left(MAX_TEXT_LENGTH);
				break;
			}
		}
		setText(text);
	}

	//
	// Обновляем родителя
	//
	if (hasParent()) {
		parent()->updateParentText();
	}
}

void ScenarioModelItem::updateParentDuration()
{
	//
	// Если есть дети - обновляем свою длительность
	//
	if (hasChildren()) {
		int duration = 0;
		foreach (ScenarioModelItem* child, m_children) {
			duration += qRound(child->duration());
		}
		setDuration(duration);
	}

	//
	// Обновляем родителя
	//
	if (hasParent()) {
		parent()->updateParentDuration();
	}
}

void ScenarioModelItem::updateParentCounter()
{
	//
	// Если есть дети - обновляем свои счётчики
	//
	if (hasChildren()) {
		Counter counter;
		foreach (ScenarioModelItem* child, m_children) {
			counter.setWords(counter.words() + child->counter().words());
			counter.setCharactersWithSpaces(
				counter.charactersWithSpaces() + child->counter().charactersWithSpaces()
				);
			counter.setCharactersWithoutSpaces(
				counter.charactersWithoutSpaces() + child->counter().charactersWithoutSpaces()
				);
		}
		setCounter(counter);
	}

	//
	// Обновляем родителя
	//
	if (hasParent()) {
		parent()->updateParentCounter();
	}
}

void ScenarioModelItem::clear()
{
	m_header.clear();
	m_text.clear();
	updateParentText();

	m_duration = 0;
	updateParentDuration();
}

//! Вспомогательные методы для организации работы модели

void ScenarioModelItem::prependItem(ScenarioModelItem* _item)
{
	//
	// Устанавливаем себя родителем
	//
	_item->m_parent = this;

	//
	// Добавляем элемент в список детей
	//
	m_children.prepend(_item);
}

void ScenarioModelItem::appendItem(ScenarioModelItem* _item)
{
	//
	// Устанавливаем себя родителем
	//
	_item->m_parent = this;

	//
	// Добавляем элемент в список детей
	//
	m_children.append(_item);
}

void ScenarioModelItem::insertItem(int _index, ScenarioModelItem* _item)
{
	_item->m_parent = this;
	m_children.insert(_index, _item);
}

void ScenarioModelItem::removeItem(ScenarioModelItem* _item)
{
	_item->clear();

	//
	// removeOne - удаляет объект при помощи delete, так что потом самому удалять не нужно
	//
	m_children.removeOne(_item);
	_item = 0;
}

bool ScenarioModelItem::hasParent() const
{
	return m_parent != 0;
}

ScenarioModelItem* ScenarioModelItem::parent() const
{
	return m_parent;
}

ScenarioModelItem* ScenarioModelItem::childAt(int _index) const
{
	return m_children.value(_index, 0);
}

int ScenarioModelItem::rowOfChild(ScenarioModelItem* _child) const
{
	return m_children.indexOf(_child);
}

int ScenarioModelItem::childCount() const
{
	return m_children.count();
}

bool ScenarioModelItem::hasChildren() const
{
	return !m_children.isEmpty();
}
