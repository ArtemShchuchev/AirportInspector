#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QWidget>
#include <QDebug>
#include <QtCharts>
#include <QChart>
#include <QChartView>
#include <QGridLayout>

namespace Ui {
class Graphic;
}


class Graphic : public QWidget
{
    Q_OBJECT

public:
    explicit Graphic(QWidget *parent = nullptr);
    ~Graphic();
    enum{
        TabYear,
        TabMonth
    };

    void addDataToLine(QVector<double> x, QVector<double> y);
    void addDataToBar(QVector<int> amount);
    void clearLine();
    void updateLine();
    void clearBar();
    void updateBar();

private slots:
    void clikedClose();
    void choiseMon(int number);
    void tabWidgetChanged(int index);

private:
    Ui::Graphic *ui;
    QChart *chart;
    QChartView *chartView;
    QGridLayout *layout;

    QBarSeries *barSer;
    QLineSeries *lineSer;
    const QStringList mon{
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
};

#endif // GRAPHIC_H
