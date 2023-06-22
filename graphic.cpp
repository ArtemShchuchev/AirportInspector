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
    //ui->cb_month->addItems(MONTH_RUS);
    ui->cb_month->setCurrentIndex(0);
    ui->cb_month->setFixedWidth(120);
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
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &Graphic::choiceTab);
}

Graphic::~Graphic()
{
    setup.saveGeometryWidget(this); // сохраняю геометрию виджета
    delete layout;
    delete chart;
    delete ui;
}

// Добавляем данные на график
void Graphic::addDataToLine(QMap<QDate, int> &statistic, QString &airportName)
{
    this->airportName = airportName;
    statYearOfDay = qMove(statistic);

    QDate date = statYearOfDay.begin().key();  // начальная дата из получ. данных
    // Наполняю comboBox полученными данными
    // выбираю даты кратные месяцам
    do{
        QString date_str = MONTH_RUS.at(date.month() - 1) + " " + QString::number(date.year());
        ui->cb_month->addItem(date_str);
        date = date.addMonths(1);
    }while(statYearOfDay.find(date) != statYearOfDay.end());


    int dateIdx = ui->cb_month->currentIndex(); // индекс даты из comboBox
    choiseMon(dateIdx);
}

// Добавляем данные на гистограмму
void Graphic::addDataToBar(QMap<QDate, int> &statistic, QString &airportName)
{
    this->airportName = airportName;
    QList<QBarSet *> listOfColumns; // столбики гистограммы

    for (auto it = statistic.begin(); it != statistic.end(); ++it){
        auto date = it.key();
        QString date_str = MONTH_RUS.at(date.month() - 1) + " " + QString::number(date.year());

        // выделяю память под столбики гистограммы
        listOfColumns.append( new QBarSet(date_str) );
        // вношу данные в новые столбики
        listOfColumns.last()->append(it.value());

        //qDebug() << date_str << " : " << it.value();
    }

    barSer->append(listOfColumns);
}

int Graphic::getCurrTab()
{
    return ui->tabWidget->currentIndex();
}

void Graphic::chartPrepear()
{
    ui->lb_airportName->setText("Аэропорт \"" + airportName + "\"");
    // 0(TabYear): за год; 1(TabMonth): за месяц.
    int index = ui->tabWidget->currentIndex();

    if (index == TabYear){
        // Устанавливаю менеджер компановки
        ui->wid_barSeries->setLayout(layout);

        if (chart->series().contains(lineSer)){
            chart->removeSeries(lineSer);
        }

        chart->addSeries(barSer); // Добавляем серию к графику
    }
    else if (index == TabMonth){
        // Устанавливаю менеджер компановки
        ui->wid_lineSeries->setLayout(layout);

        if (chart->series().contains(barSer)){
            chart->removeSeries(barSer);
        }

        chart->addSeries(lineSer); // Добавляем серию к графику
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
    // если серия не пустая - очистить данные
    if (barSer->count()){
        barSer->clear();
    }
    // если серия не пустая - очистить данные
    if (lineSer->count()){
        lineSer->clear();
    }
    if (chart->series().contains(lineSer)){
        chart->removeSeries(lineSer);
    }
    if (chart->series().contains(barSer)){
        chart->removeSeries(barSer);
    }
    close();
}

// слот обработки выбора из выпадающего списка месяцев
// наполняет серию в зависимости от выбора даты
void Graphic::choiseMon(int dateIdx)
{
    QDate date = statYearOfDay.begin().key();  // начальная дата из получ. данных

    date = date.addMonths(dateIdx);   // текущая дата выбрана из comboBox
    //qDebug() << date;

    // итератор на начало месяца
    auto itBeginMonth = statYearOfDay.find(date);
    // итератор на след. месяц
    auto itEndMonth = statYearOfDay.find(date.addMonths(1));
    // если серия не пустая - очистить данные
    if (lineSer->count()){
        lineSer->clear();
    }

    for (auto it(itBeginMonth); it != itEndMonth; ++it){
        //qDebug() << it.key() << "\t" << it.value();
        lineSer->append(it.key().day(), it.value());
    }
}

// слот переключения вкладки tabWidget
// или click по кнопке "График" на главном окне
void Graphic::choiceTab(int tabIdx)
{
    if (tabIdx == TabYear){
        if (barSer->count() == 0){
            emit sig_requestData(tabIdx);
        }
        else{
            chartPrepear();
        }
    }
    else if (tabIdx == TabMonth){
        if (lineSer->count() == 0){
            emit sig_requestData(tabIdx);
        }else{
            chartPrepear();
        }
    }
}
