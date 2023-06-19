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
    void requestToDB(const RequestType reqType, const QDate data = QDate(0, 0, 0),
                     const QString &airportCode = "");
    void connectToDataBase(const ConnectData &dbConnData);
    QSqlError getLastError();
    QSqlQueryModel* getModel(const RequestType reqType);

private:
    QSqlDatabase* db;
    QHash<RequestType, QSqlQueryModel*> model;

    void addDataBase(const QString &driver);

signals:
   void sig_SendStatusConnection(bool);
   void sig_SendStatusRequest(const QString& err);
};

#endif // DATABASE_H
