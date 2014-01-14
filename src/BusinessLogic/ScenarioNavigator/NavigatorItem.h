#ifndef NAVIGATORITEM_H
#define NAVIGATORITEM_H

#include <QString>
#include <QList>
#include <QPixmap>
#include <QTextBlock>


/**
 * @brief Класс элемента навигатора
 */
class NavigatorItem
{
public:
	NavigatorItem();
	~NavigatorItem();

	bool hasParent() const;
	NavigatorItem* parent() const;
	NavigatorItem* childAt(int _index) const;
	int rowOfChild(NavigatorItem* _child) const;
	int childCount() const;

	QPixmap icon() const;
	QString header() const;
	QString description() const;
	int timing() const;

	QTextBlock headerBlock() const;
	QTextBlock endBlock() const;

	bool isFolder() const;

	void setHeaderBlock(const QTextBlock& _block);
	void setEndBlock(const QTextBlock& _block);

	void updateItem();

	void append(NavigatorItem* _item);

private:
	NavigatorItem* m_parent;
	QList<NavigatorItem*> m_children;

	QTextBlock m_headerBlock;
	QTextBlock m_endBlock;

	QString m_header;
	QString m_description;
	int m_timing;
};

#endif // NAVIGATORITEM_H
