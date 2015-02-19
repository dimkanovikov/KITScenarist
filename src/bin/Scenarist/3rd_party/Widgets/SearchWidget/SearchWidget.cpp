#include "SearchWidget.h"

#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>


SearchWidget::SearchWidget(QWidget* _parent) :
	QFrame(_parent),
	m_editor(0),
	m_searchText(new QLineEdit(this)),
	m_prevMatch(new QPushButton(this)),
	m_nextMatch(new QPushButton(this)),
	m_replaceText(new QLineEdit(this)),
	m_replaceOne(new QPushButton(this)),
	m_replaceAll(new QPushButton(this))
{
	setFrameShape(QFrame::Box);
	setStyleSheet("*[searchWidget=\"true\"] {"
				  "  border: 0px solid black; "
				  "  border-top-width: 1px; "
				  "  border-top-style: solid; "
				  "  border-top-color: palette(dark);"
				  "}"
				  );
	setProperty("searchWidget", true);

	setFocusProxy(m_searchText);
	connect(m_searchText, SIGNAL(textChanged(QString)), this, SLOT(aboutFindNext()));
	connect(m_searchText, SIGNAL(returnPressed()), this, SLOT(aboutFindNext()));
	m_searchText->setPlaceholderText(tr("Find..."));

	m_prevMatch->setMaximumWidth(30);
	m_prevMatch->setText("◀");
	m_prevMatch->setShortcut(QKeySequence("Shift+F3"));
	connect(m_prevMatch, SIGNAL(clicked()), this, SLOT(aboutFindPrev()));

	m_nextMatch->setMaximumWidth(30);
	m_nextMatch->setText("▶");
	m_nextMatch->setShortcut(QKeySequence("F3"));
	connect(m_nextMatch, SIGNAL(clicked()), this, SLOT(aboutFindNext()));

	m_replaceText->setPlaceholderText(tr("Replace with..."));

	m_replaceOne->setText(tr("Replace"));
	connect(m_replaceOne, SIGNAL(clicked()), this, SLOT(aboutReplaceOne()));

	m_replaceAll->setText(tr("All"));
	connect(m_replaceAll, SIGNAL(clicked()), this, SLOT(aboutReplaceAll()));

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setSpacing(1);
	layout->addWidget(m_searchText);
	layout->addWidget(m_prevMatch);
	layout->addWidget(m_nextMatch);
	layout->addSpacing(16);
	layout->addWidget(m_replaceText);
	layout->addWidget(m_replaceOne);
	layout->addWidget(m_replaceAll);

	setLayout(layout);
}

void SearchWidget::setEditor(QTextEdit* _editor)
{
	if (m_editor != _editor) {
		m_editor = _editor;
	}
}

void SearchWidget::aboutFindNext()
{
	findText(false);
}

void SearchWidget::aboutFindPrev()
{
	findText(true);
}

void SearchWidget::aboutReplaceOne()
{
	const QString replaceText = m_replaceText->text();
	if (!replaceText.isEmpty()
		&& m_editor) {

		const QString searchText = m_searchText->text();
		QTextCursor cursor = m_editor->textCursor();
		if (cursor.hasSelection()
			&& cursor.selectedText() == searchText) {
			cursor.insertText(replaceText);
			aboutFindNext();
		}
	}
}

void SearchWidget::aboutReplaceAll()
{
	const QString replaceText = m_replaceText->text();
	if (!replaceText.isEmpty()
		&& m_editor) {

		aboutFindNext();
		QTextCursor cursor = m_editor->textCursor();
		while (cursor.hasSelection()) {
			cursor.insertText(replaceText);
			aboutFindNext();
			cursor = m_editor->textCursor();
		}
	}
}

void SearchWidget::findText(bool _backward)
{
	const QString searchText = m_searchText->text();
	if (!searchText.isEmpty()
		&& m_editor != 0) {
		//
		// Поиск осуществляется от позиции курсора
		//
		QTextCursor cursor = m_editor->textCursor();
		if (searchText != m_lastSearchText) {
			cursor.setPosition(qMin(cursor.selectionStart(),cursor.selectionEnd()));
		}

		//
		// Настроить направление поиска
		//
		QTextDocument::FindFlags findFlags;
		if (_backward) {
			findFlags |= QTextDocument::FindBackward;
		}

		//
		// Поиск
		//
		cursor = m_editor->document()->find(searchText, cursor, findFlags);
		if (!cursor.isNull()) {
			m_editor->setTextCursor(cursor);
		} else {
			//
			// Если достигнут конец, или начало документа зацикливаем поиск
			//
			cursor = m_editor->textCursor();
			cursor.movePosition(_backward ? QTextCursor::End : QTextCursor::Start);
			cursor = m_editor->document()->find(searchText, cursor, findFlags);
			if (!cursor.isNull()) {
				m_editor->setTextCursor(cursor);
			}
		}
	}

	//
	// Сохраняем искомый текст
	//
	m_lastSearchText = searchText;
}
