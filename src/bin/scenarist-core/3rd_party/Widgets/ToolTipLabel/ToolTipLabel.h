#ifndef TOOLTIPLABEL_H
#define TOOLTIPLABEL_H

#include <QLabel>


/**
 * @brief Метка, при наведении мыши на которую, сразу же отображается тултип
 */
class ToolTipLabel : public QLabel
{
public:
	explicit ToolTipLabel(QWidget* _parent = 0);

protected:
	void mouseMoveEvent(QMouseEvent* _event);
};

#endif // TOOLTIPLABEL_H
