#ifndef NOTETREEITEMWIDGET_HPP
#define NOTETREEITEMWIDGET_HPP

#include <QWidget>
#include <libdeeper/libdeeper.hpp>

namespace Ui {
class NoteTreeItemWidget;
}

class NoteTreeItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteTreeItemWidget(QWidget *parent = nullptr);
    ~NoteTreeItemWidget();

    QSharedPointer<deeper::Note> note() const;
    void setNote(const QSharedPointer<deeper::Note> &note);

    void setDatabase(const QSharedPointer<deeper::Database> &database);

signals:
    void onAddChildRequest();
    void onDeleteRequest();
    void onChange();

private slots:
    void on_text_textChanged();

    void on_title_textChanged(const QString &arg1);

private:
    Ui::NoteTreeItemWidget *ui;

    QSharedPointer<deeper::Database> m_database;
    QSharedPointer<deeper::Note> m_note;

    void updateUI();
    void resizeTextField();

    void resizeEvent(QResizeEvent *event) override;
};

#endif // NOTETREEITEMWIDGET_HPP
