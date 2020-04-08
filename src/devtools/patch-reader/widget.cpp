#include "widget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QTextEdit *input = new QTextEdit(this);
    QTextEdit *output = new QTextEdit(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(input);
    layout->addWidget(output);

    connect(input, &QTextEdit::textChanged, this, [=] {
        auto data = qUncompress(QByteArray::fromBase64((input->toPlainText().toUtf8())));
        output->setText(QByteArray::fromPercentEncoding(data));
    });
}

Widget::~Widget()
{
}
