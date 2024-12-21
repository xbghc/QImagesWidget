#include "qimageswidget.h"
#include "./ui_qimageswidget.h"

QImagesWidget::QImagesWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QImagesWidget)
{
    ui->setupUi(this);
}

QImagesWidget::~QImagesWidget()
{
    delete ui;
}
