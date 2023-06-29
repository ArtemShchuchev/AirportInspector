#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setup.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , reqType(requestNull)
{
    ui->setupUi(this);

    setup.restoreGeometryWidget(this, QRect(0, 0, 520, 330));

    msg = new QMessageBox(this);
    graphic = new Graphic();
    db = new DataBase(setup.getDbDriver(), this);

    connect(db, &DataBase::sig_SendStatusConnection, this, &MainWindow::receiveStatusConnectionToDB);
    connect(db, &DataBase::sig_SendStatusRequest, this, &MainWindow::receiveStatusRequestToDB);
    connect(ui->rb_in, &QRadioButton::toggled, this, [&]{
        ui->lb_data->setText("Дата прибытия");
        ui->lb_choiceAirp->setText("Аэропорт прибытия"); });
    connect(ui->rb_out, &QRadioButton::toggled, this, [&]{
        ui->lb_data->setText("Дата вылета");
        ui->lb_choiceAirp->setText("Аэропорт отбытия"); });
    connect(ui->pb_reciveRace, &QPushButton::clicked, this, &MainWindow::reciveFlightSchedule);
    connect(ui->pb_busyAirport, &QPushButton::clicked, graphic, &Graphic::choiceTab);
    connect(graphic, &Graphic::sig_requestData, this, &MainWindow::resiveRequestData);

    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->lb_data->setFixedWidth(100);
    ui->rb_out->toggle();
    setEnableControl(false);
    setStatusConnectToGUI(false);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connectToDB();
}

MainWindow::~MainWindow()
{
    setup.saveGeometryWidget(this);

    delete graphic;
    delete ui;
}

void MainWindow::ScreenDataFromDB()
{
    QSqlQueryModel *model(db->getModel(reqType));

    switch (reqType) {
    case requestListAirports:
        setEnableControl(true);
        ui->cb_Airport->setModel(model);
        break;

    case requestInAirplans:
        model->setHeaderData(0, Qt::Horizontal, tr("Номер\nрейса"));
        model->setHeaderData(1, Qt::Horizontal, tr("Время\nприбытия"));
        model->setHeaderData(2, Qt::Horizontal, tr("Аэропорт\nотправления"));
        ui->tableView->setModel(model);
        break;

    case requestOutAirplans:
        model->setHeaderData(0, Qt::Horizontal, tr("Номер\nрейса"));
        model->setHeaderData(1, Qt::Horizontal, tr("Время\nвылета"));
        model->setHeaderData(2, Qt::Horizontal, tr("Аэропорт\nназначения"));
        ui->tableView->setModel(model);
        break;

    case requestStatisticEveryMonth:
    case requestStatisticEveryDay:
    {
        QMap<QDate, int> statistic;

        int rowMax(model->rowCount());
        for (int row(0); row < rowMax; ++row){
            auto keyIdx = model->index(row, 1);
            auto dataIdx = model->index(row, 0);

            QDate key = model->data(keyIdx).toDate();
            int data = model->data(dataIdx).toInt();

            statistic.insert(key, data);
        }

        QString name = ui->cb_Airport->currentText();
        Tab idx = (reqType == requestStatisticEveryMonth) ? TabYear : TabMonth;
        graphic->addData(idx, statistic, name);

        graphic->choiceTab();
        break;
    }

    default:
        break;
    }
    reqType = requestNull;
}

void MainWindow::receiveStatusConnectionToDB(bool status)
{
    setStatusConnectToGUI(status);

    if(status){
        reqType = requestListAirports;
        auto reqDb = [&]{ db->requestToDB(reqType); };
        auto runRequest = QtConcurrent::run(reqDb);
    }
    else{
        db->disconnectFromDataBase();

        msg->setIcon(QMessageBox::Critical);
        msg->setStyleSheet("color: black");
        msg->setWindowTitle(tr("Ошибка подключения к БД!"));
        msg->setText(db->getLastError().text());
        msg->setStandardButtons(QMessageBox::Close);
        msg->exec();

        QTimer::singleShot(5000, this, SLOT(connectToDB()));
    }
}

void MainWindow::connectToDB()
{
    auto conDb = [this]{ db->connectToDataBase(setup.getConnData()); };
    auto runConnect = QtConcurrent::run(conDb);
}

void MainWindow::receiveStatusRequestToDB(const QString &err)
{
    if(err != "")
    {
        QMessageBox::critical(0, tr("Ошибка запроса к БД!"), err,
                              QMessageBox::StandardButton::Close);
    }else{
        ScreenDataFromDB();
    }
}

void MainWindow::reciveFlightSchedule()
{
    reqType = (ui->rb_in->isChecked()) ? requestInAirplans : requestOutAirplans;
    QString airportCode = getAirportCodeFromComboBox();

    auto reqDb = [this](const RequestType req, const QString &str, const QDate data){
        db->requestToDB(req, str, data);
    };
    auto runRequest = QtConcurrent::run(reqDb, reqType, airportCode, ui->dateEdit->date());
}

void MainWindow::resiveRequestData(int currTab)
{
    if (currTab == TabYear) reqType = requestStatisticEveryMonth;
    else if (currTab == TabMonth)  reqType = requestStatisticEveryDay;

    QString airportCode = getAirportCodeFromComboBox();

    auto reqDb = [this](const RequestType req, const QString &str){
        db->requestToDB(req, str);
    };
    auto runRequest = QtConcurrent::run(reqDb, reqType, airportCode);
}

void MainWindow::setStatusConnectToGUI(bool status)
{
    QString conn_str, color_str;
    if(status){
        conn_str = "Подключено";
        color_str = "color:green";
    }
    else{
        conn_str = "Отключено";
        color_str = "color:red";
    }

    ui->lb_statusConnect->setText(conn_str);
    ui->lb_statusConnect->setStyleSheet(color_str);
}

void MainWindow::setEnableControl(bool enable)
{
    ui->fr_radioBt->setEnabled(enable);
    ui->fr_data->setEnabled(enable);
    ui->pb_reciveRace->setEnabled(enable);
    ui->pb_busyAirport->setEnabled(enable);
}

QString MainWindow::getAirportCodeFromComboBox()
{
    QString airportCode;
    if (ui->cb_Airport->count() == 0){
        QMessageBox::critical(0, tr("Ошибка!"), "Список аэропортов пуст!",
                              QMessageBox::StandardButton::Close);
    }else{
        auto row = ui->cb_Airport->currentIndex();
        auto model = ui->cb_Airport->model();
        auto idx = model->index(row, 1);
        airportCode = model->data(idx).toString();
    }
    return airportCode;
}
