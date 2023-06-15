#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Первоначальная настройка виджетов ПИ
    ui->rb_out->setChecked(true);
    ui->fr_radioBt->setEnabled(false);
    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->fr_data->setEnabled(false);
    ui->pb_reciveRace->setEnabled(false);
    ui->pb_busyAirport->setEnabled(false);

    db = new DataBase(this);

    setup = new Setup();
    qDebug() << "Имя хоста: " << setup->getConnData()->hostName;

    tuneMainWindow* tmw = setup->restoreMainWindow();
    setGeometry(tmw->geometry);
    if (tmw->maximized){
        showMaximized();
    }
}

MainWindow::~MainWindow()
{
    setup->saveMainWindow(geometry(), isMaximized());
    delete setup;
    delete ui;
}
