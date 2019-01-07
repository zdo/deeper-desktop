#ifndef NOTESTREEWIDGET_HPP
#define NOTESTREEWIDGET_HPP

#include <QTreeWidget>

#include <libdeeper/libdeeper.hpp>
#include "NoteTreeItemWidget.hpp"

class NotesTreeWidget : public QTreeWidget
{
public:
    NotesTreeWidget(QWidget *parent = nullptr);

    void setCategory(const QSharedPointer<deeper::Category> &category,
                     const QSharedPointer<deeper::Database> &database);

public slots:
    void refresh();

private:
    QSharedPointer<deeper::Database> m_database;
    QSharedPointer<deeper::Category> m_category;

    void dropEvent(QDropEvent *event) override;

    void addTreeItem(QSharedPointer<deeper::Note> note, QTreeWidgetItem *parent = nullptr, bool recursive = true);
    NoteTreeItemWidget * createWidget(QTreeWidgetItem *item);
};

#endif // NOTESTREEWIDGET_HPP
