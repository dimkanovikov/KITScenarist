#ifndef UICONFIGURATOR_H
#define UICONFIGURATOR_H

#include <QWidget>

namespace Ui {
	class UIConfigurator;
}


/**
 * @brief Класс для удобства работы дизайнера с программой
 */
class UIConfigurator : public QWidget
{
	Q_OBJECT

public:
	explicit UIConfigurator(QWidget *parent = 0);
	~UIConfigurator();

private:
	void initView();
	void initConnections();
	void applyConfiguration();

	void updateLineEditBGColor();
	void configureLineEditBGColor();

private:
	QWidget* m_parentWidget;
	Ui::UIConfigurator *ui;
};

#endif // UICONFIGURATOR_H
