#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QUrl url("file:///tmp/a.json");
    m_database = QSharedPointer<deeper::Database>::create(url);

    connect(m_database.get(), &deeper::Database::onRefresh, this, [=]() {
        qDebug() << "Refreshed";
        qDebug() << "Root categories count" << m_database->categoriesTree().count();

        ui->categoriesTreeWidget->refresh();
    });

    ui->categoriesTreeWidget->setDatabase(m_database);
    m_database->refresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}
