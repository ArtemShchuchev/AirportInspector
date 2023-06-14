#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    db = new QSqlDatabase();
}

DataBase::~DataBase()
{
    delete db;
}
