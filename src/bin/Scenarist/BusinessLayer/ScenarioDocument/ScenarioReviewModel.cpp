#include "ScenarioReviewModel.h"

#include "ScenarioTextDocument.h"
#include "ScenarioTemplate.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/PasswordStorage.h>

#include <QDateTime>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>

using BusinessLogic::ScenarioReviewModel;
using BusinessLogic::ScenarioTextDocument;
using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioTemplateFacade;

namespace {
	/**
	 * @brief Получить имя пользователя
	 */
	static QString userName() {
		QString username =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/user-name",
					DataStorageLayer::SettingsStorage::ApplicationSettings);
		return username;
	}
}


ScenarioReviewModel::ScenarioReviewModel(ScenarioTextDocument *_parent) :
	QAbstractListModel(_parent),
	m_document(_parent)
{
	Q_ASSERT(_parent);

	connect(m_document, SIGNAL(contentsChange(int,int,int)), this, SLOT(aboutUpdateReviewModel(int,int,int)));
}

int ScenarioReviewModel::rowCount(const QModelIndex& _parent) const
{
	Q_UNUSED(_parent);

	return m_reviewMarks.count();
}

QVariant ScenarioReviewModel::data(const QModelIndex& _index, int _role) const
{
	QVariant result;

	if (_index.isValid()) {
		const ReviewMarkInfo info = m_reviewMarks.at(_index.row());

		switch (_role) {
			case Qt::DecorationRole: {
				if (info.isDone) {
					result = QColor(Qt::gray);
				} else {
					result = info.background.isValid() ? info.background : info.foreground;
				}
				break;
			}
			case Qt::ForegroundRole: result = info.foreground; break;
			case Qt::BackgroundRole: result = info.background; break;
			case IsDoneRole: result = info.isDone; break;
			case CommentsRole: result = info.comments; break;
			case CommentsAuthorsRole: result = info.authors; break;
			case CommentsDatesRole: result = info.dates; break;
			default: break;
		}
	}

	return result;
}

bool ScenarioReviewModel::removeRows(int _row, int _count, const QModelIndex& _parent)
{
	bool result = false;

	int last = _row + _count - 1;
	if (m_reviewMarks.size() > last) {
		beginRemoveRows(_parent, _row, last);
		for (int replies = _count; replies > 0; --replies) {
			const ReviewMarkInfo review = m_reviewMarks.takeAt(_row);
			//
			// Восстановим формат
			//
			if (!m_document->isEmpty()) {
				QTextCursor cursor(m_document);
				cursor.setPosition(review.startPosition);
				cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, review.length);
				cursor.setCharFormat(cursor.block().charFormat());

				emit reviewChanged();
			}
		}
		endRemoveRows();

		result = true;
	}

	return result;
}

void ScenarioReviewModel::setReviewMarkTextColor(int _startPosition, int _length, const QColor& _color)
{
	QTextCursor cursor(m_document);
	cursor.setPosition(_startPosition);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, _length);
	if (cursor.charFormat().foreground() != _color) {
		QTextCharFormat format;
		format.setProperty(ScenarioBlockStyle::PropertyIsReviewMark, true);
		format.setForeground(_color);
		format.setProperty(ScenarioBlockStyle::PropertyComments, QStringList() << "");
		format.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, QStringList() << ::userName());
		format.setProperty(ScenarioBlockStyle::PropertyCommentsDates, QStringList() << QDateTime::currentDateTime().toString(Qt::ISODate));
		cursor.mergeCharFormat(format);

		emit reviewChanged();
	}
}

void ScenarioReviewModel::setReviewMarkTextBgColor(int _startPosition, int _length, const QColor& _color)
{

	QTextCursor cursor(m_document);
	cursor.setPosition(_startPosition);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, _length);
	if (cursor.charFormat().background() != _color) {
		QTextCharFormat format;
		format.setProperty(ScenarioBlockStyle::PropertyIsReviewMark, true);
		format.setBackground(_color);
		//
		// Принудительно стираем цвет текста
		//
		format.setForeground(QColor());
		format.setProperty(ScenarioBlockStyle::PropertyComments, QStringList() << "");
		format.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, QStringList() << ::userName());
		format.setProperty(ScenarioBlockStyle::PropertyCommentsDates, QStringList() << QDateTime::currentDateTime().toString(Qt::ISODate));
		cursor.mergeCharFormat(format);

		emit reviewChanged();
	}
}

void ScenarioReviewModel::setReviewMarkTextHighlight(int _startPosition, int _length, const QColor& _color)
{
	QTextCursor cursor(m_document);
	cursor.setPosition(_startPosition);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, _length);
	if (cursor.charFormat().background() != _color) {
		QTextCharFormat format;
		format.setProperty(ScenarioBlockStyle::PropertyIsReviewMark, true);
		format.setProperty(ScenarioBlockStyle::PropertyIsHighlight, true);
		format.setBackground(_color);
		//
		// Принудительно стираем цвет текста
		//
		format.setForeground(QColor());
		format.setProperty(ScenarioBlockStyle::PropertyComments, QStringList() << "");
		format.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, QStringList() << ::userName());
		format.setProperty(ScenarioBlockStyle::PropertyCommentsDates, QStringList() << QDateTime::currentDateTime().toString(Qt::ISODate));
		cursor.mergeCharFormat(format);

		emit reviewChanged();
	}
}

void ScenarioReviewModel::setReviewMarkComment(int _startPosition, int _length, const QString& _comment)
{

	QTextCursor cursor(m_document);
	cursor.setPosition(_startPosition);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, _length);
	if (cursor.charFormat().toolTip() != _comment) {
		QTextCharFormat format;
		format.setProperty(ScenarioBlockStyle::PropertyIsReviewMark, true);
		format.setProperty(ScenarioBlockStyle::PropertyComments, QStringList() << _comment);
		format.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, QStringList() << ::userName());
		format.setProperty(ScenarioBlockStyle::PropertyCommentsDates, QStringList() << QDateTime::currentDateTime().toString(Qt::ISODate));
		cursor.mergeCharFormat(format);

		emit reviewChanged();
	}
}

void ScenarioReviewModel::setReviewMarkComment(const QModelIndex& _index, const QString& _comment)
{
	if (_index.isValid()) {
		const ReviewMarkInfo mark = m_reviewMarks.at(_index.row());
		setReviewMarkComment(mark.startPosition, mark.length, _comment);
	}
}

void ScenarioReviewModel::setReviewMarkIsDone(int _cursorPosition, bool _isDone)
{
	const QModelIndex indexForUpdate = indexForPosition(_cursorPosition);
	setReviewMarkIsDone(indexForUpdate, _isDone);
}

void ScenarioReviewModel::setReviewMarkIsDone(const QModelIndex& _index, bool _isDone)
{
	if (_index.isValid()) {
		const ReviewMarkInfo mark = m_reviewMarks.at(_index.row());

		QTextCursor cursor(m_document);
		cursor.setPosition(mark.startPosition);
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, mark.length);
		if (cursor.charFormat().boolProperty(ScenarioBlockStyle::PropertyIsDone) != _isDone) {
			QTextCharFormat format;
			format.setProperty(ScenarioBlockStyle::PropertyIsReviewMark, true);
			format.setProperty(ScenarioBlockStyle::PropertyIsDone, _isDone);
			cursor.mergeCharFormat(format);

			emit reviewChanged();
		}
	}
}

void ScenarioReviewModel::addReviewMarkComment(const QModelIndex& _index, const QString& _comment)
{
	if (_index.isValid()) {
		const ReviewMarkInfo mark = m_reviewMarks.at(_index.row());

		QTextCursor cursor(m_document);
		cursor.setPosition(mark.startPosition);
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, mark.length);

		QTextCharFormat format = cursor.charFormat();
		QStringList comments = format.property(ScenarioBlockStyle::PropertyComments).toStringList();
		QStringList authors = format.property(ScenarioBlockStyle::PropertyCommentsAuthors).toStringList();
		QStringList dates = format.property(ScenarioBlockStyle::PropertyCommentsDates).toStringList();

		comments << _comment;
		authors << ::userName();
		dates << QDateTime::currentDateTime().toString(Qt::ISODate);

		format.setProperty(ScenarioBlockStyle::PropertyComments, comments);
		format.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, authors);
		format.setProperty(ScenarioBlockStyle::PropertyCommentsDates, dates);
		cursor.mergeCharFormat(format);

		emit reviewChanged();
	}
}

void ScenarioReviewModel::updateReviewMarkComment(const QModelIndex& _index, int _commentIndex, const QString& _comment)
{
	if (_index.isValid()) {
		const ReviewMarkInfo mark = m_reviewMarks.at(_index.row());

		QTextCursor cursor(m_document);
		cursor.setPosition(mark.startPosition);
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, mark.length);

		QTextCharFormat format = cursor.charFormat();
		QStringList comments = format.property(ScenarioBlockStyle::PropertyComments).toStringList();
		QStringList authors = format.property(ScenarioBlockStyle::PropertyCommentsAuthors).toStringList();
		QStringList dates = format.property(ScenarioBlockStyle::PropertyCommentsDates).toStringList();

		if (comments.size() > _commentIndex) {
			comments[_commentIndex] = _comment;
			dates[_commentIndex] = QDateTime::currentDateTime().toString(Qt::ISODate);
		}

		format.setProperty(ScenarioBlockStyle::PropertyComments, comments);
		format.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, authors);
		format.setProperty(ScenarioBlockStyle::PropertyCommentsDates, dates);
		cursor.mergeCharFormat(format);

		emit reviewChanged();
	}
}

void ScenarioReviewModel::removeMark(int _cursorPosition)
{
	removeMark(indexForPosition(_cursorPosition));
}

void ScenarioReviewModel::removeMark(const QModelIndex& _index, int _commentIndex)
{
	if (_index.isValid()) {
		if (_commentIndex == 0) {
			removeRow(_index.row());
		} else {
			const ReviewMarkInfo mark = m_reviewMarks.at(_index.row());

			QTextCursor cursor(m_document);
			cursor.setPosition(mark.startPosition);
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, mark.length);

			QTextCharFormat format = cursor.charFormat();
			QStringList comments = format.property(ScenarioBlockStyle::PropertyComments).toStringList();
			QStringList authors = format.property(ScenarioBlockStyle::PropertyCommentsAuthors).toStringList();
			QStringList dates = format.property(ScenarioBlockStyle::PropertyCommentsDates).toStringList();

			if (comments.size() > _commentIndex) {
				comments.removeAt(_commentIndex);
				authors.removeAt(_commentIndex);
				dates.removeAt(_commentIndex);
			}

			format.setProperty(ScenarioBlockStyle::PropertyComments, comments);
			format.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, authors);
			format.setProperty(ScenarioBlockStyle::PropertyCommentsDates, dates);
			cursor.mergeCharFormat(format);

			emit reviewChanged();
		}
	}
}

int ScenarioReviewModel::markStartPosition(const QModelIndex& _index) const
{
	int startPosition = 0;
	if (_index.isValid()) {
		startPosition = m_reviewMarks.at(_index.row()).startPosition;
	}
	return startPosition;
}

int ScenarioReviewModel::markLength(const QModelIndex& _index) const
{
	int length = 0;
	if (_index.isValid()) {
		length = m_reviewMarks.at(_index.row()).length;
	}
	return length;
}

QModelIndex ScenarioReviewModel::indexForPosition(int _position)
{
	QModelIndex result;
	QMap<int, int>::const_iterator iterator = m_reviewMap.lowerBound(_position);
	if (iterator != m_reviewMap.end()) {
		const int reviewIndex = iterator.value();
		const ReviewMarkInfo info = m_reviewMarks.value(reviewIndex);
		if (info.startPosition <= _position
			&& (info.startPosition + info.length) >= _position) {
			result = index(reviewIndex, 0);
		}
	}
	return result;
}

void ScenarioReviewModel::aboutUpdateReviewModel(int _position, int _removed, int _added)
{
	//
	// Отрезаем ложные срабатывания
	//
	if (_position == 0
		&& _removed == _added
		&& _removed == m_document->characterCount()) {
		return;
	}

	//
	// Ищем начало изменения
	//
	int startMarkIndex = 0;
	for (; startMarkIndex < m_reviewMarks.size(); ++startMarkIndex) {
		if (m_reviewMarks.at(startMarkIndex).endPosition() >= _position) {
			break;
		}
	}


	//
	// Если есть заметки на обработку
	//
	if (startMarkIndex < m_reviewMarks.size()) {
		//
		// Прорабатываем удаление
		//
		if (_removed > 0) {
			const int removeEndPosition = _position + _removed;
			for (int markIndex = startMarkIndex; markIndex < m_reviewMarks.size(); ++markIndex) {
				ReviewMarkInfo& mark = m_reviewMarks[markIndex];
				//
				// Скорректировать размер заметки, чей конец или тело попадает под удаление
				//
				if (mark.startPosition < _position
					&& mark.endPosition() > _position) {
					//
					// ... тело
					//
					if (mark.endPosition() > removeEndPosition) {
						mark.length -= _removed;
					}
					//
					// ... конец
					//
					else {
						mark.length -= mark.endPosition() - _position;
					}
				}
				//
				// Удалить заметки полностью входящие в удалённый текст
				//
				else if (mark.startPosition >= _position
						 && mark.endPosition() <= removeEndPosition) {
					//
					// Уведомляем клиента об удалении элементов модели
					//
					beginRemoveRows(QModelIndex(), markIndex, markIndex);
					m_reviewMarks.removeAt(markIndex);
					endRemoveRows();
					--markIndex;
				}
				//
				// Скорректировать позицию заметки следующей за удалённым текстом
				//
				else {
					mark.startPosition -= _removed;
				}
			}
		}


		//
		// Прорабатываем добавление
		//
		if (_added > 0) {
			for (int markIndex = startMarkIndex; markIndex < m_reviewMarks.size(); ++markIndex) {
				ReviewMarkInfo& mark = m_reviewMarks[markIndex];
				//
				// Скорректировать размер, если текст вставлен внутри заметки
				//
				if (mark.startPosition < _position && mark.endPosition() >= _position) {
					mark.length += _added;
				}
				//
				// Скорректировать позицию заметки после вставленного текст
				//
				else if (mark.startPosition > _position) {
					mark.startPosition += _added;
				}
			}
		}
	}


	//
	// Корректируем карту текущих элементов
	//
	m_reviewMap.clear();
	for (int markIndex = 0; markIndex < m_reviewMarks.size(); ++markIndex) {
		const ReviewMarkInfo& mark = m_reviewMarks[markIndex];
		m_reviewMap.insert(mark.startPosition, markIndex);
		m_reviewMap.insert(mark.endPosition(), markIndex);
	}


	//
	// Поиск и добавление новых
	//
	if (_added > 0) {
		QTextCursor cursor(m_document);
		cursor.setPosition(_position);
		while (cursor.position() < (_position + _added)
			   && !cursor.atEnd()) {
			const QTextBlock currentBlock = cursor.block();
			if (!currentBlock.textFormats().isEmpty()) {
				foreach (const QTextLayout::FormatRange& range, currentBlock.textFormats()) {
					if (range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark)) {
						//
						// Если такой заметки не сохранено, добавляем
						//
						const int startPosition = currentBlock.position() + range.start;
						if (!m_reviewMap.contains(startPosition)) {
							int insertPosition = m_reviewMarks.size();
							QMap<int, int>::iterator iter = m_reviewMap.lowerBound(startPosition);
							if (iter != m_reviewMap.end()) {
								insertPosition = iter.value();
							}

							beginInsertRows(QModelIndex(), insertPosition, insertPosition);

							ReviewMarkInfo newMark;
							if (range.format.hasProperty(QTextFormat::BackgroundBrush)) {
								newMark.background = range.format.background().color();
							}
							if (range.format.hasProperty(QTextFormat::ForegroundBrush)) {
								newMark.foreground = range.format.foreground().color();
							}
							newMark.startPosition = currentBlock.position() + range.start;
							newMark.length = range.length;
							newMark.isDone = range.format.boolProperty(ScenarioBlockStyle::PropertyIsDone);
							newMark.comments = range.format.property(ScenarioBlockStyle::PropertyComments).toStringList();
							newMark.authors = range.format.property(ScenarioBlockStyle::PropertyCommentsAuthors).toStringList();
							newMark.dates = range.format.property(ScenarioBlockStyle::PropertyCommentsDates).toStringList();
							m_reviewMarks.insert(insertPosition, newMark);
							//
							m_reviewMap.insert(newMark.startPosition, insertPosition);
							m_reviewMap.insert(newMark.endPosition(), insertPosition);
							while (iter != m_reviewMap.end()) {
								*iter = iter.value() + 1;
								++iter;
							}

							endInsertRows();
						}
					}
				}
			}
			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.movePosition(QTextCursor::NextBlock);
		}
	}
}


int ScenarioReviewModel::ReviewMarkInfo::endPosition() const
{
	return startPosition + length - 1;
}
