#include "widget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>

#include "../../bin/scenarist-core/3rd_party/Helpers/DiffMatchPatchHelper.h"


Widget::Widget(QWidget *parent)
    : QTabWidget(parent)
{
    QWidget* uncompressPage = new QWidget;
    {
        QTextEdit *input = new QTextEdit(uncompressPage);
        QTextEdit *output = new QTextEdit(uncompressPage);
        output->setReadOnly(true);
        QHBoxLayout *layout = new QHBoxLayout(uncompressPage);
        layout->addWidget(input);
        layout->addWidget(output);

        connect(input, &QTextEdit::textChanged, this, [=] {
            auto data = qUncompress(QByteArray::fromBase64((input->toPlainText().toUtf8())));
            output->setText(QByteArray::fromPercentEncoding(data));
        });
    }
    addTab(uncompressPage, "Uncompress patch");

    //

    QWidget* applyPatchPage = new QWidget;
    {
        QTextEdit *textInput = new QTextEdit(applyPatchPage);
        QTextEdit *patchInput = new QTextEdit(applyPatchPage);
        QTextEdit *output = new QTextEdit(applyPatchPage);
        output->setReadOnly(true);
        QVBoxLayout *leftLayout = new QVBoxLayout;
        leftLayout->addWidget(textInput);
        leftLayout->addWidget(patchInput);
        QHBoxLayout *layout = new QHBoxLayout(applyPatchPage);
        layout->addLayout(leftLayout);
        layout->addWidget(output);

        auto applyPatch = [=] {
            if (textInput->toPlainText().isEmpty()) {
                return;
            }

            output->setText(DiffMatchPatchHelper::applyPatchXml(textInput->toPlainText(),
                                                                patchInput->toPlainText()));
        };
        connect(textInput, &QTextEdit::textChanged, this, applyPatch);
        connect(patchInput, &QTextEdit::textChanged, this, applyPatch);
    }
    addTab(applyPatchPage, "Apply patch");
}

Widget::~Widget()
{
}
