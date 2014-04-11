#include "ScenarioModelItem.h"

#include <QPainter>

using namespace BusinessLogic;


namespace {
	const int MAX_TEXT_LENGTH = 100;
}

ScenarioModelItem::ScenarioModelItem(QUuid _uuid) :
	m_uuid(_uuid.isNull() ? QUuid::createUuid() : _uuid),
	m_number(0),
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

QUuid ScenarioModelItem::uuid() const
{
	return m_uuid;
}

int ScenarioModelItem::number() const
{
	return m_number;
}

void ScenarioModelItem::setNumber(int _number)
{
	if (m_number != _number) {
		m_number = _number;
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

QString ScenarioModelItem::synopsis() const
{
	return m_synopsis;
}

QString ScenarioModelItem::text() const
{
	return m_text;
}

void ScenarioModelItem::setText(const QString& _text)
{
	if (m_text != _text) {
		m_text = _text.left(MAX_TEXT_LENGTH);
		updateParentText();
	}
}

int ScenarioModelItem::duration() const
{
	return m_duration;
}

void ScenarioModelItem::setDuration(int _duration)
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
			iconPath = ":/Graphics/Icons/scene.png";
			break;
		}

		case SceneGroup: {
			iconPath = ":/Graphics/Icons/scene_group.png";
			break;
		}

		case Folder: {
			iconPath = ":/Graphics/Icons/folder.png";
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

	QImage iconImage(iconPath);

	//
	// Если есть заметка, дополним иконку элемента
	//
	if (hasNote()) {
		QPainter painter(&iconImage);
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

		//
		// Отрисовываем картинку в правом нижнем углу
		//
		QImage note (":/Graphics/Icons/note.png");
		painter.drawImage(iconImage.width() - note.width(),
						  iconImage.height() - note.height(),
						  note);
	}

	return QPixmap::fromImage(iconImage);
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

void ScenarioModelItem::updateParentText()
{
	//
	// Если есть дети - обновляем свой текст
	//
	if (hasChildren()) {
		QString text;
		foreach (ScenarioModelItem* child, m_children) {
			text += child->header() + " " + child->text() + " ";
			if (text.length() > 100) {
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
			duration += child->duration();
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
