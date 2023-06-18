#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , reqType(requestNull)
{
    ui->setupUi(this);

    msg = new QMessageBox(this);
    setup = new Setup();
    setup->restoreGeometryWidget(this); // восстанавливаю геометрию mainwindow

    db = new DataBase(setup->getDbDriver(), this);
    connectToDB();

    // Первоначальная настройка виджетов ПИ
    ui->rb_out->setChecked(true);
    ui->fr_radioBt->setEnabled(false);
    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->fr_data->setEnabled(false);
    ui->pb_reciveRace->setEnabled(false);
    ui->pb_busyAirport->setEnabled(false);
    ui->lb_statusConnect->setText(NOT_CONNECT_str);
    ui->lb_statusConnect->setStyleSheet("color:red");

    // сигналы
    connect(db, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);
    connect(db, &DataBase::sig_SendStatusRequest, this, &MainWindow::ReceiveStatusRequestToDB);
}

MainWindow::~MainWindow()
{
    setup->saveGeometryWidget(this); // сохраняю геометрию mainwindow
    delete setup;
    delete ui;
}

void MainWindow::ScreenDataFromDB()
{
    QSqlQueryModel *model(db->getModel());
    switch (reqType) {
    case requestListAirports:
        ui->fr_radioBt->setEnabled(true);
        ui->fr_data->setEnabled(true);
        ui->pb_reciveRace->setEnabled(true);
        ui->pb_busyAirport->setEnabled(true);

        ui->cb_Airport->setModel(model);
        break;

    case requestInAirplans:
    case requestOutAirplans:
        ui->tableView->setModel(model);
        // Разрешаем выделение строк
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        // Устанавливаем режим выделения лишь одной строки в таблице
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        // Устанавливаем размер колонок по содержимому
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        break;

    case requestStatisticEveryMonth:
        break;

    case requestStatisticEveryDay:
        break;

    default:
        break;
    }
    reqType = requestNull;
}

// показываю статус подключения и при необходимости запускаю еще попытку подключения
void MainWindow::ReceiveStatusConnectionToDB(bool status)
{

    if(status)
    {
        msg->accept();
        ui->lb_statusConnect->setText(CONNECT_str);
        ui->lb_statusConnect->setStyleSheet("color:green");

        reqType = requestListAirports;
        auto reqDb = [&]{ db->requestToDB(reqType); };
        auto runRequest = QtConcurrent::run(reqDb);
    }
    else
    {
        db->disconnectFromDataBase();
        ui->lb_statusConnect->setText(NOT_CONNECT_str);
        ui->lb_statusConnect->setStyleSheet("color:red");

        msg->setIcon(QMessageBox::Critical);
        msg->setStyleSheet("color: black");
        msg->setWindowTitle(tr("Ошибка подключения к БД!"));
        msg->setText(db->getLastError().text());
        msg->setStandardButtons(QMessageBox::Close);
        msg->exec();

        QTimer::singleShot(5000, this, SLOT(connectToDB()));
    }
}

// пытаемся подключиться к БД
void MainWindow::connectToDB()
{
    if(ui->lb_statusConnect->text() == CONNECT_str){
        return;
    }

    msg->setIcon(QMessageBox::Information);
    msg->setStyleSheet("color: blue");
    msg->setWindowTitle("Подключение к БД");
    msg->setText("Подключаюсь...");
    msg->setStandardButtons(QMessageBox::NoButton);
    msg->show();

    auto conDb = [this]{ db->connectToDataBase(setup->getConnData()); };
    auto runConnect = QtConcurrent::run(conDb);
}

void MainWindow::ReceiveStatusRequestToDB(const QString &err)
{
    if(err != "")
    {
        QMessageBox::critical(0, tr("Ошибка запроса к БД!"), err,
                              QMessageBox::StandardButton::Close);
    }else{
        ScreenDataFromDB();
    }
}

// кнопка: Получить список рейсов
void MainWindow::on_pb_reciveRace_clicked()
{
    reqType = (ui->rb_in->isChecked()) ? requestInAirplans : requestOutAirplans;

    auto row = ui->cb_Airport->currentIndex();
    auto model = ui->cb_Airport->model();
    auto idx = model->index(row, 1);
    QString airportCode = model->data(idx).toString();

    auto reqDb = [&]{ db->requestToDB(reqType, airportCode, ui->dateEdit->date()); };
    auto runRequest = QtConcurrent::run(reqDb);
}

