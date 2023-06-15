#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>


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
