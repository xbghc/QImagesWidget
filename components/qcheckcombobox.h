#ifndef QCHECKCOMBOBOX_H
#define QCHECKCOMBOBOX_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

class QCheckComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit QCheckComboBox(QWidget *parent = nullptr);
    ~QCheckComboBox();
protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        updateLayout();
    }

private:
    QTextEdit* text;
    QPushButton* button;
    void resizeEvent();
    void updateLayout();

    constexpr static QSize BUTTON_SIZE = QSize(24, 24);

    void initButton();
};

#endif // QCHECKCOMBOBOX_H
