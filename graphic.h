#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QWidget>
#include <QDebug>
#include <QtCharts>
#include <QChart>

namespace Ui {
class Graphic;
}

class Graphic : public QWidget
{
    Q_OBJECT

public:
    explicit Graphic(QWidget *parent = nullptr);
    ~Graphic();

private slots:
    void on_pushButton_clicked();
    void choiseMon(int number);

private:
    Ui::Graphic *ui;
    QChart *chart;
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
