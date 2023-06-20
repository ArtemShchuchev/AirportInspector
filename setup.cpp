#include "setup.h"

Setup setup;

Setup::Setup()
{
    setting = new QSettings("config.ini", QSettings::IniFormat);
}

Setup::~Setup()
{
    delete setting;
}

void Setup::saveDbConnData(ConnectData &dbConnData)
{
    setting->beginGroup("DbConnData");

    setting->setValue("hostName", dbConnData.hostName);
    setting->setValue("dbName", dbConnData.dbName);
    setting->setValue("login", dbConnData.login);
    setting->setValue("pass", dbConnData.pass);
    setting->setValue("port", dbConnData.port);

    setting->endGroup();
}

const ConnectData Setup::getConnData()
{
    bool exist;
    ConnectData dbConnData;
    setting->sync();    // синхронизирует с фалом прикаждом обращении сюда
    setting->beginGroup("DbConnData");
    {
        exist = (   setting->contains("hostName") &&
                    setting->contains("dbName") &&
                    setting->contains("login") &&
                    setting->contains("pass") &&
                    setting->contains("port")
                    );
        dbConnData.hostName = setting->value("hostName", "981757-ca08998.tmweb.ru").toString();
        dbConnData.dbName = setting->value("dbName", "demo").toString();
        dbConnData.login = setting->value("login", "netology_usr_cpp").toString();
        dbConnData.pass = setting->value("pass", "CppNeto3").toString();
        dbConnData.port = setting->value("port", 5432).toInt();
    }
    setting->endGroup();

    if (exist == false){
        saveDbConnData(dbConnData);
    }

    return dbConnData;
}

const QString Setup::getDbDriver()
{
    const QString grName("dbDriver"),
            key("postgres");

    setting->beginGroup(grName);
    bool exist = setting->contains(key);
    QString dbDriver = setting->value(key, "QPSQL").toString();
    setting->endGroup();

    if (exist == false){
        setting->beginGroup(grName);
        setting->setValue(key, dbDriver);
        setting->endGroup();
    }

    return dbDriver;
}

void Setup::saveGeometryWidget(const QWidget *widget)
{
    setting->beginGroup(widget->objectName());

    bool maximized(widget->isMaximized());
    setting->setValue("maximized", maximized);
    if (maximized == false){
        setting->setValue("geometry", widget->geometry());
    }

    setting->endGroup();
}

void Setup::restoreGeometryWidget(QWidget *widget, const QRect &rect)
{
    setting->beginGroup(widget->objectName());

    widget->setGeometry(setting->value("geometry", rect).toRect());
    bool maximized(setting->value("maximized", false).toBool());
    if (maximized){
        widget->showMaximized();
    }

    setting->endGroup();
}
