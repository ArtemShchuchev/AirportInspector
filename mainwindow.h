#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QMessageBox>
#include <QtConcurrent>
#include <QTimer>

#include "database.h"
#include "setup.h"

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

public slots:


private slots:
    void ReceiveStatusConnectionToDB(bool status);
    void connectToDB();
    void ReceiveStatusRequestToDB(const QString &err);

    void on_pb_reciveRace_clicked();

    void on_rb_in_clicked();

    void on_rb_out_clicked();

private:
    Ui::MainWindow *ui;

    DataBase *db;
    Setup *setup;
    QMessageBox *msg;
    const QString   CONNECT_str = "Подключено",
                    NOT_CONNECT_str = "Отключено";
    RequestType reqType;
};
#endif // MAINWINDOW_H
