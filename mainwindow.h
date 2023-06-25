#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QMessageBox>
#include <QtConcurrent>
#include <QTimer>

#include "database.h"
#include "graphic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ScreenDataFromDB();

private slots:
    void receiveStatusConnectionToDB(bool status);
    void connectToDB();
    void receiveStatusRequestToDB(const QString &err);
    void reciveFlightSchedule();
    void resiveRequestData(int currTab);

private:
    Ui::MainWindow *ui;
    DataBase *db;
    QMessageBox *msg;
    Graphic *graphic;
    RequestType reqType;

    void setStatusConnectToGUI(bool status);
    void setEnableControl(bool enable);
    QString getAirportCodeFromComboBox();
};
#endif // MAINWINDOW_H
