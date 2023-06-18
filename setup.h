#ifndef SETUP_H
#define SETUP_H

#include <QString>
#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QWidget>
#include <QDebug>
#include <QDate>


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
    QSettings* setting;

    void saveDbConnData(ConnectData &dbConnData);

public:
    Setup();
    ~Setup();

    const ConnectData getConnData();
    const QString getDbDriver();
    void saveGeometryWidget(const QWidget *widget);
    void restoreGeometryWidget(QWidget *widget);
};

#endif // SETUP_H
