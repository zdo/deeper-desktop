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

    auto removeSrcItemFromParent = [=]() {
        auto srcItemParent = srcItem->parent();
        if (srcItemParent) {
            srcItem->parent()->takeChild(srcItemParent->indexOfChild(srcItem));
        } else {
            this->takeTopLevelItem(this->indexOfTopLevelItem(srcItem));
        }
    };

    // What kind of operation do we want to do?
    auto dropIndicator = this->dropIndicatorPosition();
    if (dropIndicator == OnViewport) {
        if (!m_database->setCategoryParent(srcCategory, nullptr)) {
            return;
        }
        removeSrcItemFromParent();
        this->addTopLevelItem(srcItem);
    } else if (!dstCategory.isNull()) {
        switch (dropIndicator) {
        case OnItem:
            if (!m_database->setCategoryParent(srcCategory, dstCategory)) {
                return;
            }
            removeSrcItemFromParent();
            dstItem->addChild(srcItem);
            break;
        case AboveItem:
        case BelowItem:
        {
            auto parent = m_database->parentOfCategory(dstCategory);
            int dstIndex = parent.isNull()
                    ? m_database->categoriesTree().indexOf(dstCategory)
                    : parent->children().indexOf(dstCategory);
            int targetIndex = dstIndex + ((dropIndicator == AboveItem) ? 0 : 1);
            if (!m_database->setCategoryParent(srcCategory, parent, targetIndex)) {
                return;
            }

            removeSrcItemFromParent();
            int finalIndex = parent.isNull()
                    ? m_database->categoriesTree().indexOf(srcCategory)
                    : parent->children().indexOf(srcCategory);
            if (dstItem->parent()) {
                dstItem->parent()->insertChild(finalIndex, srcItem);
            } else {
                this->insertTopLevelItem(finalIndex, srcItem);
            }
        }
            break;
        default:
            qDebug() << "Incomplete drop indicator handling" << dropIndicator;
        }
    } else {
        qCritical() << "Empty destination category on item-related drop indicator" << dropIndicator;
    }

    std::function<void (QTreeWidgetItem *item)> fn = [&](QTreeWidgetItem *item) {
        this->createWidget(item);

        for (int i = 0; i < item->childCount(); ++i) {
            fn(item->child(i));
        }
    };

    if (srcItem->parent()) {
        srcItem->parent()->setExpanded(true);
    }
    while (!this->selectedItems().isEmpty()) {
        this->selectedItems().first()->setSelected(false);
    }
    srcItem->setSelected(true);
    srcItem->setExpanded(true);

    fn(srcItem);

    // This line is required due to the graphical glitch that occurs when you're moving
    // some child item to the top-most root position.
    this->resizeColumnToContents(0);
}

void CategoriesTreeWidget::addTreeItem(QSharedPointer<deeper::Category> category, QTreeWidgetItem *parent, bool recursive)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    if (parent == nullptr) {
        this->addTopLevelItem(item);
    } else {
        parent->addChild(item);
    }

    item->setData(0, Qt::UserRole, category->id());
    this->createWidget(item);

    if (recursive) {
        for (auto childCategory : category->children()) {
            this->addTreeItem(childCategory, item);
        }
    }
}

CategoryTreeItemWidget *CategoriesTreeWidget::createWidget(QTreeWidgetItem *item)
{
    auto widget = new CategoryTreeItemWidget();
    this->setItemWidget(item, 0, widget);
    auto categoryId = item->data(0, Qt::UserRole).toString();
    auto category = m_database->categoryWithId(categoryId);
    widget->setCategory(category);

    connect(widget, &CategoryTreeItemWidget::onAddChildRequest, this, [=]() {
        auto newCategory = m_database->createCategory(category->id());
        this->addTreeItem(newCategory, item, false);
        this->expandItem(item);
    });
    connect(widget, &CategoryTreeItemWidget::onDeleteRequest, this, [=]() {
        m_database->deleteCategory(category);
        delete item;
    });
    connect(widget, &CategoryTreeItemWidget::onChange, this, [=]() {
        m_database->saveCategoryTree();
    });

    return widget;
}
