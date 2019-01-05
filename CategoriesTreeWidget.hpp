#ifndef CATEGORIESTREEWIDGET_HPP
#define CATEGORIESTREEWIDGET_HPP

#include <QTreeWidget>

#include <libdeeper/libdeeper.hpp>

class CategoriesTreeWidget : public QTreeWidget
{
public:
    CategoriesTreeWidget(QWidget *parent = nullptr);

    QSharedPointer<deeper::Database> database() const;
    void setDatabase(const QSharedPointer<deeper::Database> &database);

public slots:
    void refresh();

private:
    QSharedPointer<deeper::Database> m_database;

    void dropEvent(QDropEvent *event) override;

    void addTreeItem(QSharedPointer<deeper::Category> category, QTreeWidgetItem *parent = nullptr, bool recursive = true);
};

#endif // CATEGORIESTREEWIDGET_HPP
