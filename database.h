#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>

#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"

//Количество полей данных необходимых для подключения к БД
#define NUM_DATA_FOR_CONNECT_TO_DB 5

//Перечисление полей данных
enum fieldsForConnect
{
    hostName = 0,
    dbName = 1,
    login = 2,
    pass = 3,
    port = 4
};

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    void DisconnectFromDataBase(const QString& nameDb = "");
    void RequestToDB(int requestIndex);
    void ConnectToDataBase(const QVector<QString>& dataForConnect);
    QSqlError GetLastError(void);

private:
    void AddDataBase(const QString& driver, const QString& nameDB = "");

    QSqlDatabase* db;

signals:

};

#endif // DATABASE_H
