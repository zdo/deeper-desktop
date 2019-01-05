#include "CategoriesTreeWidget.hpp"
#include "CategoryTreeItemWidget.hpp"

#include <QDropEvent>

CategoriesTreeWidget::CategoriesTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

QSharedPointer<deeper::Database> CategoriesTreeWidget::database() const
{
    return m_database;
}

void CategoriesTreeWidget::setDatabase(const QSharedPointer<deeper::Database> &database)
{
    m_database = database;
    this->refresh();
}

void CategoriesTreeWidget::refresh()
{
    this->clear();

    if (m_database.isNull()) {
        return;
    }

    for (auto category : m_database->categoriesTree()) {
        this->addTreeItem(category);
    }
}

void CategoriesTreeWidget::dropEvent(QDropEvent *event)
{
    // Determine the source item.
    auto srcItem = this->currentItem();
    if (srcItem == nullptr) {
        return;
    }
    auto srcWidget = static_cast<CategoryTreeItemWidget *>(this->itemWidget(srcItem, 0));
    auto srcCategory = srcWidget->category();
    if (srcCategory == nullptr) {
        return;
    }

    // Determine the destination item.
    auto dstItem = this->itemAt(event->pos());
    CategoryTreeItemWidget *dstWidget = nullptr;
    QSharedPointer<deeper::Category> dstCategory;
    if (dstItem != nullptr) {
        dstWidget = static_cast<CategoryTreeItemWidget *>(this->itemWidget(dstItem, 0));
        dstCategory = dstWidget->category();
    }

    // What kind of operation do we want to do?
    auto dropIndicator = this->dropIndicatorPosition();
    if (dropIndicator == OnViewport) {
        m_database->setCategoryParent(srcCategory, nullptr);
        this->addTopLevelItem(srcItem);
    } else if (!dstCategory.isNull()) {
        switch (dropIndicator) {
        case OnItem:
            m_database->setCategoryParent(srcCategory, dstCategory);
            break;
        case AboveItem:
        case BelowItem:
        {
            auto parent = m_database->parentOfCategory(dstCategory);
            int dstIndex = parent.isNull()
                    ? m_database->categoriesTree().indexOf(dstCategory)
                    : parent->children().indexOf(dstCategory);
            int targetIndex = dstIndex + ((dropIndicator == AboveItem) ? 0 : 1);
            m_database->setCategoryParent(srcCategory, parent, targetIndex);
        }
            break;
        default:
            qDebug() << "Incomplete drop indicator handling" << dropIndicator;
        }
    } else {
        qCritical() << "Empty destination category on item-related drop indicator" << dropIndicator;
    }

    this->refresh();
}

void CategoriesTreeWidget::addTreeItem(QSharedPointer<deeper::Category> category, QTreeWidgetItem *parent, bool recursive)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    if (parent == nullptr) {
        this->addTopLevelItem(item);
    } else {
        parent->addChild(item);
    }

    auto widget = new CategoryTreeItemWidget();
    this->setItemWidget(item, 0, widget);
    widget->setCategory(category);

    connect(widget, &CategoryTreeItemWidget::onAddChildRequest, this, [=]() {
        auto newCategory = m_database->createCategory(category->id());
        this->addTreeItem(newCategory, item, false);
        this->expandItem(item);
    });
    connect(widget, &CategoryTreeItemWidget::onDeleteRequest, this, [=]() {
        m_database->deleteCategory(category);
        this->refresh();
    });
    connect(widget, &CategoryTreeItemWidget::onChange, this, [=]() {
        m_database->saveCategoryTree();
    });

    if (recursive) {
        for (auto childCategory : category->children()) {
            this->addTreeItem(childCategory, item);
        }
    }
}
