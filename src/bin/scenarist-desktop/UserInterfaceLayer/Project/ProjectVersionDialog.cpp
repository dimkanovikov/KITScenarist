#include "ProjectVersionDialog.h"
#include "ui_ProjectVersionDialog.h"

ProjectVersionDialog::ProjectVersionDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectVersionDialog)
{
    ui->setupUi(this);
}

ProjectVersionDialog::~ProjectVersionDialog()
{
    delete ui;
}
