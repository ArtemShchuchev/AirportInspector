#ifndef SETUP_H
#define SETUP_H

#include <QString>
#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QRect>


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

struct tuneMainWindow
{
    QRect geometry;
    bool maximized;
};


class Setup
{
private:
    QSettings* set;
    QString dbDriver;
    QString dbName;
    const uint8_t sizeConnData = 5;
    ConnectData dbConnData;
    tuneMainWindow tuneMW;

public:
    Setup();
    ~Setup();
    void saveToConfig();
    void loadFromConfig();
    ConnectData *getConnData();
    void saveMainWindow(QRect geometry, bool);
    tuneMainWindow *restoreMainWindow();
};

#endif // SETUP_H
