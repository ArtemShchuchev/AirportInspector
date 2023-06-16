#include "setup.h"
//#include "ui_mainwindow.h"

Setup::Setup()
{
    set = new QSettings("config.ini", QSettings::IniFormat);
    loadDbConnData();
}

Setup::~Setup()
{
    delete set;
}

void Setup::saveDbConnData()
{
    set->beginGroup("DbConnData");

    set->setValue("hostName", dbConnData.hostName);
    set->setValue("dbName", dbConnData.dbName);
    set->setValue("login", dbConnData.login);
    set->setValue("pass", dbConnData.pass);
    set->setValue("port", dbConnData.port);

    set->endGroup();
}

void Setup::loadDbConnData()
{
    set->beginGroup("DbConnData");
    bool exist(set->contains("hostName") &&
               set->contains("dbName") &&
               set->contains("login") &&
               set->contains("pass") &&
               set->contains("port"));

    dbConnData.hostName = set->value("hostName", "981757-ca08998.tmweb.ru").toString();
    dbConnData.dbName = set->value("dbName", "netology_cpp").toString();
    dbConnData.login = set->value("login", "netology_usr_cpp").toString();
    dbConnData.pass = set->value("pass", "CppNeto3").toString();
    dbConnData.port = set->value("port", 5432).toInt();

    set->endGroup();

    if (exist == false){
        saveDbConnData();
    }
}

ConnectData *Setup::getConnData()
{
    return &dbConnData;
}

void Setup::saveGeometryWidget(const QWidget *widget)
{
    set->beginGroup(widget->windowTitle());

    bool maximized(widget->isMaximized());
    set->setValue("maximized", maximized);
    if (maximized == false){
        set->setValue("geometry", widget->geometry());
    }

    set->endGroup();
}

void Setup::restoreGeometryWidget(QWidget *widget)
{
    set->beginGroup(widget->windowTitle());

    widget->setGeometry(set->value("geometry", QRect(0, 0, 561, 528)).toRect());
    bool maximized(set->value("maximized", false).toBool());
    if (maximized){
        widget->showMaximized();
    }

    set->endGroup();
}
