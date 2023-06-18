#include "database.h"

DataBase::DataBase(const QString &driver, QObject *parent)
    : QObject{parent}, model(nullptr)
{
    db = new QSqlDatabase();

    addDataBase(driver);
}

DataBase::~DataBase()
{
    delete db;
    if (model){
        delete model;
    }
}

void DataBase::disconnectFromDataBase()
{
    *db = QSqlDatabase::database();
    db->close();
}

void DataBase::requestToDB(const RequestType reqType, const QString &airportCode, const QDate data)
{
    if (model){
        delete model;
        model = nullptr;
    }

    QString request;
    switch (reqType) {
    case requestListAirports:
        request = "SELECT airport_name->>'ru' as \"airportName\", airport_code FROM\
                bookings.airports_data\
                order by \"airportName\"";
        break;

    case requestInAirplans:
        request = "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\"\
                from bookings.flights f\
                JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport\
                where f.arrival_airport = '" + airportCode + "'\
                and f.scheduled_arrival::date = date('"
                + QString::number(data.year()) + "-"
                + QString::number(data.month()) + "-"
                + QString::number(data.day()) + "')";
        break;

    case requestOutAirplans:
        request = "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\"\
                from bookings.flights f\
                JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport\
                WHERE f.departure_airport = '" + airportCode + "'\
                and f.scheduled_departure::date = date('"
                + QString::number(data.year()) + "-"
                + QString::number(data.month()) + "-"
                + QString::number(data.day()) + "')";
        break;

    case requestStatisticEveryMonth:
        request = "SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\"\
                from bookings.flights f\
                WHERE (scheduled_departure::date > date('2016-08-31') and\
                scheduled_departure::date <= date('2017-08-31')) and ( departure_airport =\
                airportCode or arrival_airport = airportCode )\
                group by \"Month\"";
        break;

    case requestStatisticEveryDay:
        request = "SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\"\
                from bookings.flights f\
                WHERE(scheduled_departure::date > date('2016-08-31') and\
                scheduled_departure::date <= date('2017-08-31')) and ( departure_airport =\
                airportCode or arrival_airport = airportCode)\
                GROUP BY \"Day\"";
        break;

    default:
        break;
    }

    model = new QSqlQueryModel();
    model->setQuery(request, *db);
    //model->setHeaderData(0, Qt::Horizontal, tr("Название фильма"));
    //model->setHeaderData(1, Qt::Horizontal, tr("Описание фильма"));
    //model->sort(1, Qt::AscendingOrder);

    auto err = model->lastError().text();

    emit sig_SendStatusRequest(err);
}

void DataBase::connectToDataBase(const ConnectData &dbConnData)
{
    db->setHostName(dbConnData.hostName);
    db->setDatabaseName(dbConnData.dbName);
    db->setUserName(dbConnData.login);
    db->setPassword(dbConnData.pass);
    db->setPort(dbConnData.port);

    bool open(db->open());
    emit sig_SendStatusConnection(open);
}

QSqlError DataBase::getLastError()
{
    return db->lastError();
}

QSqlQueryModel *DataBase::getModel()
{
    return model;
}

void DataBase::addDataBase(const QString &driver)
{
    *db = QSqlDatabase::addDatabase(driver);
}
