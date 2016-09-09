#ifndef TABBAREXPANDED_H
#define TABBAREXPANDED_H

#include <3rd_party/Widgets/TabBar/TabBar.h>

class TabBarExpanded : public TabBar
{
    Q_OBJECT

public:
    explicit TabBarExpanded(QWidget* _parent = 0);

protected:
    QSize tabSizeHint(int _index) const override;
};

#endif // TABBAREXPANDED_H
