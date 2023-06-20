#include "graphic.h"
#include "ui_graphic.h"
#include "setup.h"

Graphic::Graphic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphic)
{
    ui->setupUi(this);

    barSer = new QBarSeries(this);
    lineSer = new QLineSeries(this);

    // Объект QChart является основным, в котором хранятся все данные графиков
    // и который отвечает за само поле отображения графика, управляет осями,
    // легенодой и прочими атрибутами графика.
    chart = new QChart();
    chart->legend()->setVisible(false);
    // Объект QChartView является виджетом отображальщиком графика. В его
    // конструктор необходимо передать ссылку на объект QChart.
    chartView = new QChartView(chart, this);
    // Размещает виджеты по ячейкам таблицы?!
    layout = new QGridLayout();
    layout->addWidget(chartView);
    // Устанавливаю менеджер компановки для ui->wid_lineSeries
//    ui->wid_lineSeries->setLayout(layout);
//    ui->wid_barSeries->setLayout(layout);
    tabWidgetChanged(Graphic::TabYear);
    // график настроен, можно показать
    //chartView->chart()->createDefaultAxes();
    chartView->show();

    // Настройка виджета
    setup.restoreGeometryWidget(this, QRect(0, 0, 400, 300)); // восстанавливаю геометрию виджета
    setWindowModality(Qt::ApplicationModal);
    ui->cb_month->addItems(mon);
    ui->cb_month->setCurrentIndex(0);
    ui->cb_month->setFixedWidth(100);
    ui->tabWidget->setCurrentIndex(Graphic::TabYear);


    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(clikedClose()));
    connect(ui->cb_month, SIGNAL(activated(int)), this, SLOT(choiseMon(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabWidgetChanged(int)));
}

Graphic::~Graphic()
{
    setup.saveGeometryWidget(this); // сохраняю геометрию виджета
    delete layout;
    delete chart;
    delete ui;
}

// Добавляем данные на график
void Graphic::addDataToLine(QVector<double> x, QVector<double> y)
{
    // Зададим размер графика, т.е. количество точек.
    // Необходимо, чтобы х и у были равны.
    // РАВНЫ, т.к. каждой точке соотв. 1ин "x" и 1ин "y", т.е. пара координат
    uint32_t size = (x.size() >= y.size()) ? y.size() : x.size();

    // Добавление точек в серию.
    for(uint32_t i(0); i < size; ++i)
    {
        lineSer->append(x[i],y[i]);
    }
}

// Добавляем данные на гистограмму
void Graphic::addDataToBar(QVector<int> amount)
{
    auto amount_it = amount.begin();
    auto mon_it = mon.begin();

    QList<QBarSet *> listOfColumns(amount.size());

    for (auto &column : listOfColumns){
        column = new QBarSet(*mon_it++);
        column->append(*amount_it++);
    }

    barSer->append(listOfColumns);
}

// Очищаем данные на графике
void Graphic::clearLine()
{
    lineSer->clear();
    if (chart->series().contains(lineSer)){
        chart->removeSeries(lineSer);
    }
}

// Добавляем серию к графику
void Graphic::updateLine()
{
    chart->addSeries(lineSer);
}

void Graphic::clearBar()
{
    barSer->clear();
    if (chart->series().contains(barSer)){
        chart->removeSeries(barSer);
    }
}

void Graphic::updateBar()
{
    chart->addSeries(barSer);
}

// клик по кнопке "Закрыть" - закроет окно графиков
void Graphic::clikedClose()
{
    close();
}

// слот обработки выбора из выпадающего списка месяцев
void Graphic::choiseMon(int number)
{
    qDebug() << number << ": " << mon.at(number);
}

// смена вкладки на - 0(TabYear): за год; 1(TabMonth): за месяц.
void Graphic::tabWidgetChanged(int index)
{
    if (index == Graphic::TabYear){
        QVector<int> am{4, 2, 3, 5, 2, 8, 1, 5, 7, 2, 8, 4};
        addDataToBar(am);
        ui->wid_barSeries->setLayout(layout);
        updateBar();
        clearLine();
    }
    else if (index == Graphic::TabMonth){
        QVector<double> x{0, 2, 3, 5}, y{6, 4, 5, 2};
        addDataToLine(x, y);
        ui->wid_lineSeries->setLayout(layout);
        updateLine();
        clearBar();
    }
}

