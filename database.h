#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>
#include <QDate>
#include <QHash>
#include "setup.h"

enum RequestType
{
    requestNull,
    requestListAirports,
    requestInAirplans,
    requestOutAirplans,
    requestStatisticEveryMonth,
    requestStatisticEveryDay
};

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(const QString &driver, QObject *parent = nullptr);
    ~DataBase();

    void disconnectFromDataBase();
    void requestToDB(const RequestType reqType, const QString &airportCode = "",
                     const QDate data = QDate(0, 0, 0));
    void connectToDataBase(const ConnectData &dbConnData);
    QSqlError getLastError();
    QSqlQueryModel* getModel(const RequestType reqType);

private:
    QSqlDatabase* db;
    QHash<RequestType, QSqlQueryModel*> model;
    struct RequestString{
        const QString listAirports(){
            return  "SELECT airport_name->>'ru' as \"airportName\", airport_code FROM\
                    bookings.airports_data\
                    order by \"airportName\"";
        }
        const QString inAirplans(const QString &airportCode, const QDate date){
            return  "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" from bookings.flights f\
                    JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport\
                    where f.arrival_airport = '" + airportCode + "'\
                    and f.scheduled_arrival::date = date('"
                    + QString::number(date.year()) + "-"
                    + QString::number(date.month()) + "-"
                    + QString::number(date.day()) + "')\
                    order by \"Name\"";
        }
        const QString outAirplans(const QString &airportCode, const QDate date){
            return  "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" from bookings.flights f\
                    JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport\
                    WHERE f.departure_airport = '" + airportCode + "'\
                    and f.scheduled_departure::date = date('"
                    + QString::number(date.year()) + "-"
                    + QString::number(date.month()) + "-"
                    + QString::number(date.day()) + "')\
                    order by \"Name\"";
        }
        const QString statisticEveryMonth(const QString &airportCode){
            return   "SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" from bookings.flights f\
                    WHERE (scheduled_departure::date > date('2016-08-31') and\
                    scheduled_departure::date <= date('2017-08-31')) and\
                    ( departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "' )\
                    group by \"Month\"";
        }
        const QString statisticEveryDay(const QString &airportCode){
            return  "SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" from bookings.flights f\
                    WHERE(scheduled_departure::date > date('2016-08-31') and\
                    scheduled_departure::date <= date('2017-08-31')) and\
                    ( departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "')\
                    GROUP BY \"Day\"";
        }
    }req;

    void addDataBase(const QString &driver);

signals:
   void sig_SendStatusConnection(bool);
   void sig_SendStatusRequest(const QString& err);
};

#endif // DATABASE_H
