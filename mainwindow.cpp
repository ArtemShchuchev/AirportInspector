#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setup.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , reqType(requestNull)
{
    ui->setupUi(this);

    msg = new QMessageBox(this);
    graphic = new Graphic();
    //setup = new Setup();
    db = new DataBase(setup.getDbDriver(), this);

    // Первоначальная настройка виджетов ПИ
    setup.restoreGeometryWidget(this, QRect(0, 0, 520, 330)); // восстанавливаю геометрию mainwindow
    ui->fr_radioBt->setEnabled(false);
    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->fr_data->setEnabled(false);
    ui->pb_reciveRace->setEnabled(false);
    ui->pb_busyAirport->setEnabled(false);
    ui->lb_statusConnect->setText(NOT_CONNECT_str);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->lb_data->setFixedWidth(100);
    ui->rb_out->setChecked(true);
    on_rb_out_clicked();

    connectToDB();

    // сигналы
    connect(db, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);
    connect(db, &DataBase::sig_SendStatusRequest, this, &MainWindow::ReceiveStatusRequestToDB);
}

MainWindow::~MainWindow()
{
    delete graphic;
    setup.saveGeometryWidget(this); // сохраняю геометрию mainwindow
    delete ui;
}

void MainWindow::ScreenDataFromDB()
{
    QSqlQueryModel *model(db->getModel(reqType));

    switch (reqType) {
    case requestListAirports:
        ui->fr_radioBt->setEnabled(true);
        ui->fr_data->setEnabled(true);
        ui->pb_reciveRace->setEnabled(true);
        ui->pb_busyAirport->setEnabled(true);
        ui->cb_Airport->setModel(model);
        break;

    case requestInAirplans:
        model->setHeaderData(0, Qt::Horizontal, tr("Номер\nрейса"));
        model->setHeaderData(1, Qt::Horizontal, tr("Время\nприбытия"));
        model->setHeaderData(2, Qt::Horizontal, tr("Аэропорт\nотправления"));
        ui->tableView->setModel(model);
        // Разрешаем выделение строк
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        // Устанавливаем режим выделения лишь одной строки в таблице
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        // Устанавливаем размер колонок по содержимому
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        break;

    case requestOutAirplans:
        model->setHeaderData(0, Qt::Horizontal, tr("Номер\nрейса"));
        model->setHeaderData(1, Qt::Horizontal, tr("Время\nвылета"));
        model->setHeaderData(2, Qt::Horizontal, tr("Аэропорт\nназначения"));
        ui->tableView->setModel(model);
        // Разрешаем выделение строк
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        // Устанавливаем режим выделения лишь одной строки в таблице
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        // Устанавливаем размер колонок по содержимому
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

    auto conDb = [this]{ db->connectToDataBase(setup.getConnData()); };
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

    auto reqDb = [this](const RequestType req, const QDate data, const QString &str)
    {
        db->requestToDB(req, data, str);
    };
    // Конкаррент - это что то тёмное! (работает как хочу...)
    auto runRequest = QtConcurrent::run(reqDb, reqType, ui->dateEdit->date(), airportCode);
}


void MainWindow::on_rb_in_clicked()
{
    ui->lb_data->setText("Дата прибытия");
    ui->lb_choiceAirp->setText("Аэропорт прибытия");
}


void MainWindow::on_rb_out_clicked()
{
    ui->lb_data->setText("Дата вылета");
    ui->lb_choiceAirp->setText("Аэропорт отбытия");
}


void MainWindow::on_pb_busyAirport_clicked()
{
    graphic->show();
}

