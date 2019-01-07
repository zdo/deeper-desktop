#include "NotesTreeWidget.hpp"

#include <QTimer>
#include <QDropEvent>

NotesTreeWidget::NotesTreeWidget(QWidget *parent) : QTreeWidget(parent)
{

}

void NotesTreeWidget::setCategory(const QSharedPointer<deeper::Category> &category, const QSharedPointer<deeper::Database> &database)
{
    m_category = category;
    m_database = database;

    this->refresh();
}

void NotesTreeWidget::refresh()
{
    this->clear();

    if (m_database.isNull()) {
        return;
    }

    for (auto note : m_database->notes(m_category)->rootObjects()) {
        this->addTreeItem(note);
    }
}

void NotesTreeWidget::dropEvent(QDropEvent *event)
{
    // Determine the source item.
    auto srcItem = this->currentItem();
    if (srcItem == nullptr) {
        return;
    }
    auto srcWidget = static_cast<NoteTreeItemWidget *>(this->itemWidget(srcItem, 0));
    auto srcNote = srcWidget->note();
    if (srcNote == nullptr) {
        return;
    }

    // Determine the destination item.
    auto dstItem = this->itemAt(event->pos());
    NoteTreeItemWidget *dstWidget = nullptr;
    QSharedPointer<deeper::Note> dstNote;
    if (dstItem != nullptr) {
        dstWidget = static_cast<NoteTreeItemWidget *>(this->itemWidget(dstItem, 0));
        dstNote = dstWidget->note();
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
        if (!m_database->setNoteParent(srcNote, m_category, nullptr)) {
            return;
        }
        removeSrcItemFromParent();
        this->addTopLevelItem(srcItem);
    } else if (!dstNote.isNull()) {
        switch (dropIndicator) {
        case OnItem:
            if (!m_database->setNoteParent(srcNote, m_category, dstNote)) {
                return;
            }
            removeSrcItemFromParent();
            dstItem->addChild(srcItem);
            break;
        case AboveItem:
        case BelowItem:
        {
            int dstIndex = dstNote->orderIndex();
            int targetIndex = dstIndex + ((dropIndicator == AboveItem) ? 0 : 1);
            if (!m_database->setNoteParent(srcNote, m_category,
                                           m_database->notes(m_category)->parent(dstNote),
                                           targetIndex)) {
                return;
            }

            removeSrcItemFromParent();
            int finalIndex = srcNote->orderIndex();

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
    this->scheduleDelayedItemsLayout();
}

void NotesTreeWidget::addTreeItem(QSharedPointer<deeper::Note> note, QTreeWidgetItem *parent, bool recursive)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    if (parent == nullptr) {
        this->addTopLevelItem(item);
    } else {
        parent->addChild(item);
    }

    item->setData(0, Qt::UserRole, note->id());
    this->createWidget(item);

    if (recursive) {
        for (auto childNote : m_database->notes(m_category)->children(note)) {
            this->addTreeItem(childNote, item);
        }
    }

    // For some reason items will be resized incorrectly so the delay is needed here.
    QTimer::singleShot(100, this, [=]() {
        this->scheduleDelayedItemsLayout();
    });
}

NoteTreeItemWidget *NotesTreeWidget::createWidget(QTreeWidgetItem *item)
{
    auto widget = new NoteTreeItemWidget();
    widget->setDatabase(m_database);
    this->setItemWidget(item, 0, widget);

    auto noteId = item->data(0, Qt::UserRole).toString();
    auto note = m_database->notes(m_category)->objectWithId(noteId);
    widget->setNote(note);

    connect(widget, &NoteTreeItemWidget::onAddChildRequest, this, [=]() {
        auto newNote = m_database->createNote(m_category, note);
        this->addTreeItem(newNote, item, false);
        this->expandItem(item);
    });
    connect(widget, &NoteTreeItemWidget::onDeleteRequest, this, [=]() {
        m_database->deleteNote(note);
        delete item;
    });
    connect(widget, &NoteTreeItemWidget::onChange, this, [=]() {
        m_database->saveNote(note);

        this->scheduleDelayedItemsLayout();
    });

    return widget;
}
