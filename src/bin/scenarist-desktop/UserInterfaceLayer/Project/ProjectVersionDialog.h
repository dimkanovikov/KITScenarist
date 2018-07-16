#ifndef PROJECTVERSIONDIALOG_H
#define PROJECTVERSIONDIALOG_H

#include <QWidget>

namespace Ui {
    class ProjectVersionDialog;
}

class ProjectVersionDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectVersionDialog(QWidget *parent = 0);
    ~ProjectVersionDialog();

private:
    Ui::ProjectVersionDialog *ui;
};

#endif // PROJECTVERSIONDIALOG_H
