#ifndef ARROWFLOW_PROPERTIESDIALOG_H
#define ARROWFLOW_PROPERTIESDIALOG_H

#include <QDialog>
#include "arrowflow.h"

namespace Ui {
    class ArrowFlowPropertiesDialog;
}

class ArrowFlowPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
	explicit ArrowFlowPropertiesDialog(ArrowFlow *flow, QWidget *parent = 0);
    ~ArrowFlowPropertiesDialog();
private:
	ArrowFlow *flow;
    Ui::ArrowFlowPropertiesDialog *ui;
private slots:
	void on_btnOkay_clicked();
	void on_btnCancel_clicked();
};

#endif // ARROWFLOW_PROPERTIESDIALOG_H
