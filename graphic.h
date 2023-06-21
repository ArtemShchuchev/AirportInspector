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


    void addDataToLine(QVector<double> x, QVector<double> y);
    void addDataToBar(QMap<QDate, int> &statistic);
    void clearLine();
    void updateLine();
    void clearBar();
    void updateBar();
    int getCurrTab();

public slots:
    void chartPrepear();

private slots:
    void clikedClose();
    void choiseMon(int number);

private:
    Ui::Graphic *ui;
    QChart *chart;
    QChartView *chartView;
    QGridLayout *layout;

    QBarSeries *barSer;
    QLineSeries *lineSer;
    const QStringList months{
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

signals:
    void sig_requestData(int currTab);
};

#endif // GRAPHIC_H
