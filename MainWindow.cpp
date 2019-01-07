#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showMaximized();

    QUrl url("file:///tmp/a.json");
    m_database = QSharedPointer<deeper::Database>::create(url);
    m_database->refresh();

    ui->categoriesTreeWidget->setDatabase(m_database);

    connect(ui->categoriesTreeWidget, &CategoriesTreeWidget::currentItemChanged, this,
            [=](QTreeWidgetItem *current, QTreeWidgetItem */*previous*/) {
        auto category = ui->categoriesTreeWidget->category(current);

//        auto newNote = m_database->createNote(category);
//        newNote->setText(QDateTime::currentDateTime().toString());
//        m_database->saveNote(newNote);

        ui->notesTreeWidget->setCategory(category, m_database);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
