#include "NoteTreeItemWidget.hpp"
#include "ui_NoteTreeItemWidget.h"

#include <QMenu>

NoteTreeItemWidget::NoteTreeItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteTreeItemWidget)
{
    ui->setupUi(this);

    connect(ui->label, &ClickableLabel::clicked, this, [=]() {
        QMenu contextMenu(tr("Context menu"), this);

//        connect(&action, &QAction::triggered, this, [=]() {
//            qDebug() << "None";
//        });
        contextMenu.addAction(new QAction("None"));

        contextMenu.addSeparator();

        for (auto s : m_database->noteStates()) {
//            connect(&action, &QAction::triggered, this, [=]() {
//                qDebug() << s->title();
//            });
            contextMenu.addAction(new QAction(s->title()));
        }


        contextMenu.exec(mapToGlobal(ui->label->pos()));
    });
}

NoteTreeItemWidget::~NoteTreeItemWidget()
{
    delete ui;
}

QSharedPointer<deeper::Note> NoteTreeItemWidget::note() const
{
    return m_note;
}

void NoteTreeItemWidget::setNote(const QSharedPointer<deeper::Note> &note)
{
    m_note = note;
    this->updateUI();
}

void NoteTreeItemWidget::updateUI()
{
    if (m_note.isNull()) {
        return;
    }

    ui->title->setText(m_note->title());
    ui->text->setText(m_note->text());

    this->resizeTextField();
}

void NoteTreeItemWidget::resizeTextField()
{
    ui->text->setFixedHeight(int(ui->text->document()->size().height()));
}

void NoteTreeItemWidget::resizeEvent(QResizeEvent */*event*/)
{
    this->resizeTextField();
}

void NoteTreeItemWidget::on_text_textChanged()
{
    m_note->setText(ui->text->toHtml());
    this->resizeTextField();

    emit this->onChange();
}

void NoteTreeItemWidget::on_title_textChanged(const QString &arg1)
{
    m_note->setTitle(ui->title->text());
    emit this->onChange();
}

void NoteTreeItemWidget::setDatabase(const QSharedPointer<deeper::Database> &database)
{
    m_database = database;
}
