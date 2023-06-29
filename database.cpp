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

void DataBase::requestToDB(const RequestType reqType, const QString &airportCode, const QDate date)
{
    QString request;
    switch (reqType) {
    case requestListAirports:
        request = req.listAirports();
        break;

    case requestInAirplans:
        request = req.inAirplans(airportCode, date);
        break;

    case requestOutAirplans:
        request = req.outAirplans(airportCode, date);
        break;

    case requestStatisticEveryMonth:
        request = req.statisticEveryMonth(airportCode);
        break;

    case requestStatisticEveryDay:
        request = req.statisticEveryDay(airportCode);
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
