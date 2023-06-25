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

enum Tab{
    TabYear,
    TabMonth
};


class Graphic : public QWidget
{
    Q_OBJECT

public:
    explicit Graphic(QWidget *parent = nullptr);
    ~Graphic();
    void addData(Tab idx, QMap<QDate, int> &statistic, QString &airportName);

public slots:
    void choiceTab();

private slots:
    void closeGraphicWindow();
    void choiseMonth(int dateIdx);

private:
    Ui::Graphic *ui;
    QChart *chartBar, *chartLine;
    QChartView *viewBar, *viewLine;

    QBarSeries *barSer;
    QLineSeries *lineSer;

    QBarSet *myBarSet;
    QBarCategoryAxis *axisBarX;
    QValueAxis *axisBarY;
    QValueAxis *axisLineX;
    QValueAxis *axisLineY;

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
    QList<QMap<QDate, int>::ConstIterator> listDataIt;
    QStringList categories;

    void closeEvent(QCloseEvent *event) override;

signals:
    void sig_requestData(int currTab);
};

#endif // GRAPHIC_H
