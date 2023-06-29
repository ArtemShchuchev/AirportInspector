#include "database.h"

DataBase::DataBase(const QString &driver, QObject *parent)
    : QObject{parent}
{
    db = new QSqlDatabase();
    addDataBase(driver);
}

DataBase::~DataBase()
{
    delete db;
    for (auto& m : model){
        if (m){
            delete m;
        }
    }
}

void DataBase::disconnectFromDataBase()
{
    *db = QSqlDatabase::database();
    db->close();
}

void DataBase::requestToDB(const RequestType reqType, const QString &airportCode, const QDate data)
{
    QString request;
    switch (reqType) {
    case requestListAirports:
        request = "SELECT airport_name->>'ru' as \"airportName\", airport_code FROM\
                bookings.airports_data\
                order by \"airportName\"";
        break;

    case requestInAirplans:
        request = "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" from bookings.flights f\
                JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport\
                where f.arrival_airport = '" + airportCode + "'\
                and f.scheduled_arrival::date = date('"
                + QString::number(data.year()) + "-"
                + QString::number(data.month()) + "-"
                + QString::number(data.day()) + "')\
                order by \"Name\"";
        break;

    case requestOutAirplans:
        request = "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" from bookings.flights f\
                JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport\
                WHERE f.departure_airport = '" + airportCode + "'\
                and f.scheduled_departure::date = date('"
                + QString::number(data.year()) + "-"
                + QString::number(data.month()) + "-"
                + QString::number(data.day()) + "')\
                order by \"Name\"";
        break;

    case requestStatisticEveryMonth:
        request = "SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" from bookings.flights f\
                WHERE (scheduled_departure::date > date('2016-08-31') and\
                scheduled_departure::date <= date('2017-08-31')) and\
                ( departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "' )\
                group by \"Month\"";
        break;

    case requestStatisticEveryDay:
        request = "SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" from bookings.flights f\
                WHERE(scheduled_departure::date > date('2016-08-31') and\
                scheduled_departure::date <= date('2017-08-31')) and\
                ( departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "')\
                GROUP BY \"Day\"";
        break;

    default:
        break;
    }

    if (model.contains(reqType) == false){
        model.insert(reqType, new QSqlQueryModel());
    }
    model.value(reqType)->setQuery(request, *db);
    auto err = model.value(reqType)->lastError().text();

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

QSqlQueryModel *DataBase::getModel(const RequestType reqType)
{
    return model.value(reqType, nullptr);
}

void DataBase::addDataBase(const QString &driver)
{
    *db = QSqlDatabase::addDatabase(driver);
}
