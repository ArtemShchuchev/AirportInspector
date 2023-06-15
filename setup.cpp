#include "setup.h"

Setup::Setup()
{
    set = new QSettings("config.ini", QSettings::IniFormat);
    loadFromConfig();
}

Setup::~Setup()
{
    delete set;
}

void Setup::saveToConfig()
{
    set->beginGroup("Settings_DataBase");
    set->setValue("hostName", dbConnData.hostName);
    set->setValue("dbName", dbConnData.dbName);
    set->setValue("login", dbConnData.login);
    set->setValue("pass", dbConnData.pass);
    set->setValue("port", dbConnData.port);
    set->endGroup();
}

void Setup::loadFromConfig()
{
    set->beginGroup("Settings_DataBase");
    dbConnData.hostName = set->value("hostName").toString();
    dbConnData.dbName = set->value("dbName").toString();
    dbConnData.login = set->value("login").toString();
    dbConnData.pass = set->value("pass").toString();
    dbConnData.port = set->value("port").toInt();
    set->endGroup();
}

ConnectData *Setup::getConnData()
{
    return &dbConnData;
}

void Setup::saveMainWindow(QRect geometry, bool maximized)
{
    set->beginGroup("mainwindow");
    if (maximized){
        set->setValue("maximized", maximized);
    }
    else{
        set->setValue("geometry", geometry);
        set->setValue("maximized", maximized);
    }

    set->endGroup();
}

tuneMainWindow *Setup::restoreMainWindow()
{
    set->beginGroup("mainwindow");
    tuneMW.geometry = set->value("geometry").toRect();
    tuneMW.maximized = set->value("maximized").toBool();
    set->endGroup();
    return &tuneMW;
}
