#ifndef SHAPE_ACTION_PROPERTIESDIALOG_H
#define SHAPE_ACTION_PROPERTIESDIALOG_H

#include <QDialog>
#include "card.h"

namespace Ui {
    class ActionShapePropertiesDialog;
}

class ActionShapePropertiesDialog : public QDialog
{
    Q_OBJECT
public:
	explicit ActionShapePropertiesDialog(CardShape *shp, QWidget *parent = 0);
    ~ActionShapePropertiesDialog();
private:
	CardShape *shape;
    Ui::ActionShapePropertiesDialog *ui;
private slots:
	void on_btnCancel_clicked();
 void on_btnOkay_clicked();
};

#endif // SHAPE_ACTION_PROPERTIESDIALOG_H
