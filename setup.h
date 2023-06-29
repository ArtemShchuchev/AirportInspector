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
#include <mutex>


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
    static Setup * pinstance_;
    static std::mutex mutex_;

protected:
    Setup();
    ~Setup();
    QSettings* setting;
    void saveDbConnData(ConnectData &dbConnData);

public:
    Setup(Setup &other) = delete;
    void operator=(const Setup &) = delete;
    static Setup *GetInstance();

    const ConnectData getConnData();
    const QString getDbDriver();
    void saveGeometryWidget(const QWidget *widget);
    void restoreGeometryWidget(QWidget *widget, const QRect &rect);
};

#endif // SETUP_H
