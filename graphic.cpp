#include "graphic.h"
#include "ui_graphic.h"
#include "setup.h"

Graphic::Graphic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphic)
{
    ui->setupUi(this);

    // Настройка виджета
    setup.restoreGeometryWidget(this, QRect(0, 0, 400, 300)); // восстанавливаю геометрию виджета
    setWindowModality(Qt::ApplicationModal);
    ui->cb_month->addItems(months);
    ui->cb_month->setCurrentIndex(0);
    ui->cb_month->setFixedWidth(100);
    ui->tabWidget->setCurrentIndex(TabYear);


    // Объекты построения графиков
    barSer = new QBarSeries(this);      // столбиковый
    lineSer = new QLineSeries(this);    // линейный
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


    //tabWidgetChanged(Graphic::TabYear);
    // график настроен, можно показать
    //chartView->chart()->createDefaultAxes();
    //chartView->show();



    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(clikedClose()));
    connect(ui->cb_month, SIGNAL(activated(int)), this, SLOT(choiseMon(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(chartPrepear()));

//    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [&](int tab){
//        emit sig_requestData(tab);
//    });
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
void Graphic::addDataToBar(QMap<QDate, int> &statistic)
{
    QList<QBarSet *> listOfColumns; // столбики гистограммы

    auto endit = statistic.end();
    for (auto it = statistic.begin(); it != endit; ++it){
        QString mon = months[it.key().month() - 1] + " " + QString::number(it.key().year());

        // выделяю память под столбики гистограммы
        listOfColumns.append( new QBarSet(mon) );
        // вношу данные в новые столбики
        listOfColumns.last()->append(it.value());

        qDebug() << mon << " : " << it.value();
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
    //barSer->clear();
    if (chart->series().contains(barSer)){
        chart->removeSeries(barSer);
    }
}

void Graphic::updateBar()
{
    chart->addSeries(barSer);
}

int Graphic::getCurrTab()
{
    return ui->tabWidget->currentIndex();
}

void Graphic::chartPrepear()
{
    // 0(TabYear): за год; 1(TabMonth): за месяц.
    int index = ui->tabWidget->currentIndex();

    if (index == TabYear){
//        QVector<int> am{4, 2, 3, 5, 2, 8, 1, 5, 7, 2, 8, 4};
//        addDataToBar(am);
        // Устанавливаю менеджер компановки
        ui->wid_barSeries->setLayout(layout);
        updateBar();
        clearLine();
    }
    else if (index == TabMonth){
        QVector<double> x{0, 2, 3, 5}, y{6, 4, 5, 2};
        addDataToLine(x, y);
        // Устанавливаю менеджер компановки
        ui->wid_lineSeries->setLayout(layout);
        updateLine();
        clearBar();
    }

    qDebug() << "graphic show...";

    // график настроен, можно показать
    //chartView->chart()->createDefaultAxes();
    //chartView->show();
    show();
}

// клик по кнопке "Закрыть" - закроет окно графиков
void Graphic::clikedClose()
{
    close();
}

// слот обработки выбора из выпадающего списка месяцев
void Graphic::choiseMon(int number)
{
    qDebug() << number << ": " << months.at(number);
}
