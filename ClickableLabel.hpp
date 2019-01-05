#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR) : QLabel(parent) {

    }

    ~ClickableLabel() override {}

signals:
    void clicked();
    void doubleClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit this->clicked();
        event->ignore();
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override {
        emit this->doubleClicked();
        event->ignore();
    }
};

#endif // CLICKABLELABEL_H
