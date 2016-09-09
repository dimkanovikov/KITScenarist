#include "TabBarExpanded.h"

TabBarExpanded::TabBarExpanded(QWidget* _parent) : TabBar(_parent)
{

}

QSize TabBarExpanded::tabSizeHint(int _index) const
{
    if (count() != 0) {
        return QSize(width() / count(), height());
    } else {
        return TabBar::tabSizeHint(_index);
    }
}
