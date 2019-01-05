#include "CategoryTreeItemWidget.hpp"
#include "ui_CategoryTreeItemWidget.h"

CategoryTreeItemWidget::CategoryTreeItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoryTreeItemWidget)
{
    ui->setupUi(this);

    ui->addChildBtn->setVisible(false);
    ui->delBtn->setVisible(false);

    ui->title->setVisible(true);
    ui->titleEdit->setVisible(false);

    connect(ui->title, &ClickableLabel::doubleClicked, this, [=]() {
        ui->title->setVisible(false);
        ui->titleEdit->setVisible(true);

        ui->titleEdit->setFocus();
        ui->titleEdit->selectAll();
    });
}

CategoryTreeItemWidget::~CategoryTreeItemWidget()
{
    delete ui;
}

QSharedPointer<deeper::Category> CategoryTreeItemWidget::category() const
{
    return m_category;
}

void CategoryTreeItemWidget::setCategory(const QSharedPointer<deeper::Category> &category)
{
    m_category = category;
    if (m_category.isNull()) {
        return;
    }

    ui->title->setText(m_category->title());
    ui->titleEdit->setText(m_category->title());
}

void CategoryTreeItemWidget::enterEvent(QEvent */*event*/)
{
    ui->addChildBtn->setVisible(true);
    ui->delBtn->setVisible(true);
}

void CategoryTreeItemWidget::leaveEvent(QEvent */*event*/)
{
    ui->addChildBtn->setVisible(false);
    ui->delBtn->setVisible(false);
}

void CategoryTreeItemWidget::on_addChildBtn_clicked()
{
    emit this->onAddChildRequest();
}

void CategoryTreeItemWidget::on_delBtn_clicked()
{
    emit this->onDeleteRequest();
}

void CategoryTreeItemWidget::on_titleEdit_textChanged()
{
    m_category->setTitle(ui->titleEdit->text());
    ui->title->setText(m_category->title());

    emit this->onChange();
}

void CategoryTreeItemWidget::on_titleEdit_returnPressed()
{
    this->parentWidget()->setFocus();

    ui->title->setVisible(true);
    ui->titleEdit->setVisible(false);
}

void CategoryTreeItemWidget::on_titleEdit_inputRejected()
{
    ui->titleEdit->setText(m_category->title());

    ui->title->setVisible(true);
    ui->titleEdit->setVisible(false);
}

void CategoryTreeItemWidget::on_titleEdit_editingFinished()
{
    this->on_titleEdit_returnPressed();
}
