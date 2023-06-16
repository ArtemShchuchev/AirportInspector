#ifndef SETUP_H
#define SETUP_H

#include <QString>
#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QWidget>
#include <QDebug>


//#define POSTGRE_DRIVER "QPSQL"
//#define DB_NAME "MyDB"


// Структура полей подключения к БД
struct ConnectData
{
    QString hostName;
    QString dbName;
    QString login;
    QString pass;
    uint16_t port;
};


class Setup
{
private:
    QSettings* set;
    QString dbDriver;
    QString dbName;
    const uint8_t sizeConnData = 5;
    ConnectData dbConnData;

public:
    Setup();
    ~Setup();
    void saveDbConnData();
    void loadDbConnData();
    ConnectData *getConnData();
    void saveGeometryWidget(const QWidget *widget);
    void restoreGeometryWidget(QWidget *widget);
};

#endif // SETUP_H
