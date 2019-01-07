#ifndef CATEGORYTREEITEMWIDGET_HPP
#define CATEGORYTREEITEMWIDGET_HPP

#include <QWidget>
#include <libdeeper/libdeeper.hpp>

namespace Ui {
class CategoryTreeItemWidget;
}

class CategoryTreeItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CategoryTreeItemWidget(QWidget *parent = nullptr);
    ~CategoryTreeItemWidget() override;

    QSharedPointer<deeper::Category> category() const;
    void setCategory(const QSharedPointer<deeper::Category> &category);



signals:
    void onAddChildRequest();
    void onDeleteRequest();
    void onChange();

private slots:
    void on_addChildBtn_clicked();

    void on_delBtn_clicked();

    void on_titleEdit_textChanged();

    void on_titleEdit_returnPressed();

    void on_titleEdit_inputRejected();

    void on_titleEdit_editingFinished();

private:
    Ui::CategoryTreeItemWidget *ui;
    QSharedPointer<deeper::Category> m_category;

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // CATEGORYTREEITEMWIDGET_HPP
