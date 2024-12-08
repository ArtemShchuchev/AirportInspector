#include "graphic.h"
#include "ui_graphic.h"
#include "setup.h"

Graphic::Graphic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphic),
    axisLineX(nullptr), axisLineY(nullptr),
    axisBarX(nullptr), axisBarY(nullptr),
    lineSer(nullptr), barSer(nullptr)
{
    ui->setupUi(this);

    Setup::GetInstance()->restoreGeometryWidget(this, QRect(0, 0, 500, 400));
    setWindowModality(Qt::ApplicationModal);
    ui->cb_month->setFixedWidth(120);
    ui->tabWidget->setCurrentIndex(TabYear);

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
    Setup::GetInstance()->saveGeometryWidget(this);

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
        if (barSer == nullptr){
            barSer = new QBarSeries(this);
            chartBar->addSeries(barSer);
            chartBar->setAnimationOptions(QChart::SeriesAnimations);
        }

        int max(0);
        QBarSet *myBarSet = new QBarSet("Bar", this);
        for (auto it = statistic.begin(); it != statistic.end(); ++it){
            auto date = it.key();
            QString date_str = MONTH_RUS.at(date.month() - 1) + " " + QString::number(date.year());

            categories.append(date_str);
            myBarSet->append(it.value());

            if (max < it.value()){
                max = it.value();
            }
        }
        barSer->append(myBarSet);

        if (axisBarX == nullptr){
            axisBarX = new QBarCategoryAxis(this);
            chartBar->addAxis(axisBarX, Qt::AlignBottom);
            axisBarX->setLabelsAngle(270);
        }
        axisBarX->append(categories);
        barSer->attachAxis(axisBarX);

        if (axisBarY == nullptr){
            axisBarY = new QValueAxis(this);
            chartBar->addAxis(axisBarY, Qt::AlignLeft);
            axisBarY->setTitleText("Кол-во прилетов/вылетов");
            axisBarY->setLabelFormat("%u");
        }
        axisBarY->setRange(0, max);
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

    categories.clear();
    listDataIt.clear();

    if (barSer->count()){
        barSer->detachAxis(axisBarX);
        barSer->detachAxis(axisBarY);
        barSer->clear();
    }
    if (lineSer && lineSer->count()){
        lineSer->detachAxis(axisLineX);
        lineSer->detachAxis(axisLineY);
        lineSer->clear();
    }
}

void Graphic::choiseMonth(int dateIdx)
{
    if (lineSer == nullptr){
        lineSer = new QLineSeries(this);
        chartLine->addSeries(lineSer);
        chartLine->setAnimationOptions(QChart::SeriesAnimations);
    }
    if (lineSer->count()){
        lineSer->detachAxis(axisLineX);
        lineSer->detachAxis(axisLineY);
        lineSer->clear();
    }

    auto currentMonth_it = listDataIt.at(dateIdx);
    auto nextMonth_it = listDataIt.at(dateIdx + 1);
    QDate date;
    int max(0), min((std::numeric_limits<int>::max()));
    while(currentMonth_it != nextMonth_it)
    {
        date = currentMonth_it.key();
        int value = currentMonth_it.value();
        lineSer->append(date.day(), value);
        ++currentMonth_it;
        if (max < value){
            max = value;
        }else if (min > value){
            min = value;
        }
    }

    if (axisLineX == nullptr){
        axisLineX = new QValueAxis(this);
        chartLine->addAxis(axisLineX, Qt::AlignBottom);
        axisLineX->setTitleText("Числа месяца");
        axisLineX->setLabelFormat("%u");
        axisLineX->setTickCount(1);
    }
    axisLineX->setRange(1, date.daysInMonth());
    lineSer->attachAxis(axisLineX);

    if (axisLineY == nullptr){
        axisLineY = new QValueAxis(this);
        chartLine->addAxis(axisLineY, Qt::AlignLeft);
        axisLineY->setTitleText("Кол-во прилетов/вылетов");
        axisLineY->setLabelFormat("%u");
    }
    axisLineY->setRange(--min, ++max);
    axisLineY->setTickCount(max - min + 1);
    lineSer->attachAxis(axisLineY);
}

void Graphic::choiceTab()
{
    int tabIdx = ui->tabWidget->currentIndex();

    if ((tabIdx == TabYear && categories.empty()) ||
            (tabIdx == TabMonth && listDataIt.empty()))
    {
        emit sig_requestData(tabIdx);
    }
    else{
        show();
    }
}
