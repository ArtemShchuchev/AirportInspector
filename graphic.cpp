#include "graphic.h"
#include "ui_graphic.h"

Graphic::Graphic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphic)
{
    ui->setupUi(this);

    chart = new QChart();
    barSer = new QBarSeries();
    lineSer = new QLineSeries();

    // Настройка виджета
    setWindowModality(Qt::ApplicationModal);
    ui->cb_month->addItems(mon);
    ui->cb_month->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);



    lineSer->append(0, 6);
    lineSer->append(2, 4);
    chart->addSeries(lineSer);
    ui->gv_lineSeries = new QChartView(chart);




    connect(ui->cb_month, SIGNAL(activated(int)), this, SLOT(choiseMon(int)));
}

Graphic::~Graphic()
{
    delete ui;
}

void Graphic::on_pushButton_clicked()
{
    close();
}

// слот обработки выбора из выпадающего списка месяцев
void Graphic::choiseMon(int number)
{
    qDebug() << number << ": " << mon.at(number);
}

