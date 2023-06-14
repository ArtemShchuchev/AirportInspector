#include "mainwindow.h"
#include "database.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->rb_out->setChecked(true);
    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));

    DataBase db;
}

MainWindow::~MainWindow()
{
    delete ui;
}

