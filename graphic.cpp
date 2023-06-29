#include "graphic.h"
#include "ui_graphic.h"
#include "setup.h"

Graphic::Graphic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphic),
    axisLineX(nullptr),
    axisLineY(nullptr)
{
    ui->setupUi(this);

    setup.restoreGeometryWidget(this, QRect(0, 0, 400, 300));
    setWindowModality(Qt::ApplicationModal);
    ui->cb_month->setFixedWidth(120);
    ui->tabWidget->setCurrentIndex(TabYear);

    barSer = new QBarSeries(this);
    lineSer = new QLineSeries(this);

    chartBar = new QChart();
    chartLine = new QChart();
    chartBar->legend()->hide();
    chartLine->legend()->hide();

    viewBar = new QChartView(chartBar);
    viewLine = new QChartView(chartLine);

    ui->layoutBar->addWidget(viewBar);
    ui->layoutLine->addWidget(viewLine);


    connect(ui->pushButton, &QPushButton::clicked, this, &Graphic::closeGraphicWindow);
    connect(ui->cb_month, SIGNAL(activated(int)), this, SLOT(choiseMonth(int)));
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &Graphic::choiceTab);
}

Graphic::~Graphic()
{
    setup.saveGeometryWidget(this);

    delete chartBar;
    delete chartLine;

    delete viewBar;
    delete viewLine;

    delete ui;
}

void Graphic::addData(Tab idx, QMap<QDate, int> &statistic, QString &airportName)
{
    ui->lb_airportName->setText("Аэропорт \"" + airportName + "\"");

    switch (idx) {
    case TabYear:
    {
        myBarSet = new QBarSet("Bar", this);
        int max(0);

        for (auto it = statistic.begin(); it != statistic.end(); ++it){
            auto date = it.key();
            QString date_str = MONTH_RUS.at(date.month() - 1) + " " + QString::number(date.year());

            categories.append(date_str);
            myBarSet->append(it.value());

            if (max < it.value()){
                max = it.value();
            }
        }

        chartBar->addSeries(barSer);
        barSer->append(myBarSet);

        axisBarX = new QBarCategoryAxis(this);
        axisBarX->append(categories);
        axisBarX->setLabelsAngle(270);
        chartBar->addAxis(axisBarX, Qt::AlignBottom);
        barSer->attachAxis(axisBarX);

        axisBarY = new QValueAxis(this);
        axisBarY->setTitleText("Кол-во прилетов/вылетов");
        axisBarY->setLabelFormat("%u");
        axisBarY->setRange(0, max);
        chartBar->addAxis(axisBarY, Qt::AlignLeft);
        barSer->attachAxis(axisBarY);
        break;
    }

    case TabMonth:
    {
        ui->cb_month->clear();
        const int CURR_IDX(0);
        ui->cb_month->setCurrentIndex(CURR_IDX);

        statYearOfDay = qMove(statistic);

        auto it = statYearOfDay.constKeyValueBegin();
        auto end_it = statYearOfDay.constKeyValueEnd();
        int oldMonth(0);
        while(it != end_it)
        {
            int month = it.base().key().month();
            if (oldMonth != month){
                oldMonth = month;
                QString date_str = MONTH_RUS.at(month - 1) + " " + QString::number(it.base().key().year());
                ui->cb_month->addItem(date_str);
                listDataIt.append(it.base());
            }
            ++it;
        }
        listDataIt.append(end_it.base());

        choiseMonth(CURR_IDX);
        break;
    }

    default:
        break;
    }
}

void Graphic::closeGraphicWindow()
{
    close();
}

void Graphic::closeEvent(QCloseEvent *event)
{
    event->accept();

    delete axisBarY;
    axisBarY = nullptr;
    delete axisBarX;
    axisBarX = nullptr;
    delete axisLineX;
    axisLineX = nullptr;
    delete axisLineY;
    axisLineY = nullptr;

    listDataIt.clear();

    if (barSer->count()){
        barSer->clear();
    }
    if (lineSer->count()){
        lineSer->clear();
    }
    if (chartLine->series().contains(lineSer)){
        chartLine->removeSeries(lineSer);
    }
    if (chartBar->series().contains(barSer)){
        chartBar->removeSeries(barSer);
    }
}

void Graphic::choiseMonth(int dateIdx)
{
    if (lineSer->count()){
        lineSer->clear();
    }
    if (chartLine->series().contains(lineSer)){
        chartLine->removeSeries(lineSer);
    }
    if (axisLineX){
        delete axisLineX;
    }
    if (axisLineY){
        delete axisLineY;
    }

    auto currentMonth_it = listDataIt.at(dateIdx);
    auto nextMonth_it = listDataIt.at(dateIdx + 1);

    while(currentMonth_it != nextMonth_it)
    {
        lineSer->append(currentMonth_it.key().day(), currentMonth_it.value());
        ++currentMonth_it;
    }

    chartLine->addSeries(lineSer);

    axisLineX = new QValueAxis(this);
    axisLineX->setTitleText("Числа месяца");
    axisLineX->setLabelFormat("%u");
    axisLineX->setTickCount(1);
    chartLine->addAxis(axisLineX, Qt::AlignBottom);
    lineSer->attachAxis(axisLineX);

    axisLineY = new QValueAxis(this);
    axisLineY->setTitleText("Кол-во прилетов/вылетов");
    axisLineY->setLabelFormat("%u");
    axisLineY->setTickCount(5);
    chartLine->addAxis(axisLineY, Qt::AlignLeft);
    lineSer->attachAxis(axisLineY);
}

void Graphic::choiceTab()
{
    int tabIdx = ui->tabWidget->currentIndex();

    if ((tabIdx == TabYear && barSer->count() == 0) ||
            (tabIdx == TabMonth && lineSer->count() == 0))
    {
        emit sig_requestData(tabIdx);
    }
    else{
        show();
    }
}
