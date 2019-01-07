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
    m_database->refresh();

    ui->categoriesTreeWidget->setDatabase(m_database);
}

MainWindow::~MainWindow()
{
    delete ui;
}
