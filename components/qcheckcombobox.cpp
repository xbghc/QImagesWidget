#include "qcheckcombobox.h"

#include <QSizePolicy>


QCheckComboBox::QCheckComboBox(QWidget *parent)
    : QWidget(parent),
    button(new QPushButton(this)),
    text(new QTextEdit(this))
{
    initButton();

    text->setEnabled(false);
    updateLayout();
}

QCheckComboBox::~QCheckComboBox()
{
}

void QCheckComboBox::updateLayout()
{
    auto margins = contentsMargins();
    int x = width() - button->width() - margins.right();
    int y = (height() - button->height()) / 2;
    button->move(x, y);

    text->resize(this->size());
}

void QCheckComboBox::initButton()
{
    static QString BUTTON_STYLE = R"(
    QPushButton {
        border: none;
        background: #ffffff;
    }
    QPushButton:hover {
        background: #eeeeee;
    }
    QPushButton:pressed {
        background: #dddddd;
    }
)";
    button->setStyleSheet(BUTTON_STYLE);

    button->setFixedSize(BUTTON_SIZE);
    button->setIcon(QIcon(":/qcheckcombobox.svg"));
    button->raise();

}

