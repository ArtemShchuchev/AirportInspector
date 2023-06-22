#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QWidget>
#include <QDebug>
#include <QtCharts>
#include <QChart>
#include <QChartView>
#include <QGridLayout>
#include <QList>
#include <QBarSet>

namespace Ui {
class Graphic;
}

enum{
    TabYear,
    TabMonth
};


class Graphic : public QWidget
{
    Q_OBJECT

public:
    explicit Graphic(QWidget *parent = nullptr);
    ~Graphic();


    void addDataToLine(QMap<QDate, int> &statistic, QString &airportName);
    void addDataToBar(QMap<QDate, int> &statistic, QString &airportName);
    int getCurrTab();

public slots:
    void chartPrepear();
    void choiceTab(int tabIdx);

private slots:
    void clikedClose();
    void choiseMon(int dateIdx);

private:
    Ui::Graphic *ui;
    QChart *chart;
    QChartView *chartView;
    QGridLayout *layout;

    QBarSeries *barSer;
    QLineSeries *lineSer;
    const QStringList MONTH_RUS{
        "Январь",
        "Февраль",
        "Март",
        "Апрель",
        "Май",
        "Июнь",
        "Июль",
        "Август",
        "Сентябрь",
        "Октябрь",
        "Ноябрь",
        "Декабрь" };
    QMap<QDate, int> statYearOfDay;
    QString airportName;

signals:
    void sig_requestData(int currTab);
};

#endif // GRAPHIC_H
