#include "pdlg_arrowflow.h"
#include "ui_pdlg_arrowflow.h"
#include "../scene/customgraphicsscene.h"

ArrowFlowPropertiesDialog::ArrowFlowPropertiesDialog(ArrowFlow *flw, QWidget *parent)
	: flow(flw)
	, QDialog(parent)
	, ui(new Ui::ArrowFlowPropertiesDialog)
{
	ui->setupUi(this);
	ui->txtText->setPlainText(flow->text());
	ui->btnCancel->setIcon(QIcon::fromTheme("dialog-cancel"));
	ui->btnOkay->setIcon(QIcon::fromTheme("dialog-ok"));
}

ArrowFlowPropertiesDialog::~ArrowFlowPropertiesDialog()
{
	delete ui;
}

void ArrowFlowPropertiesDialog::on_btnOkay_clicked()
{
	flow->emitStateIsAboutToBeChangedByUser();
	flow->setText(ui->txtText->toPlainText());
	accept();
}

void ArrowFlowPropertiesDialog::on_btnCancel_clicked()
{
	reject();
}
