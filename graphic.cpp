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
    ui->cb_month->setFixedWidth(120);
    ui->tabWidget->setCurrentIndex(TabYear);

    // Объекты построения графиков
    barSer = new QBarSeries(this);      // столбиковый
    lineSer = new QLineSeries(this);    // линейный
    // Объект QChart является основным, в котором хранятся все данные графиков
    // и который отвечает за само поле отображения графика, управляет осями,
    // легенодой и прочими атрибутами графика.
    chart = new QChart();
    chart->legend()->hide();
    //chart->createDefaultAxes();

    //axis_X = qobject_cast<QValueAxis *>(chart->axisX());
    //axis_Y = qobject_cast<QValueAxis *>(chart->axisY());
//    axis_X = new QValueAxis;
//    axis_Y = new QValueAxis;


    // Объект QChartView является виджетом отображальщиком графика. В его
    // конструктор необходимо передать ссылку на объект QChart.
    chartView = new QChartView(chart, this);
    // Размещает виджеты по ячейкам таблицы?!
    layout = new QGridLayout();
    layout->addWidget(chartView);


    connect(ui->pushButton, &QPushButton::clicked, this, &Graphic::closeGraphicWindow);
    connect(ui->cb_month, SIGNAL(activated(int)), this, SLOT(choiseMonth(int)));
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
    ui->cb_month->clear();
    const int CURR_IDX(0);
    ui->cb_month->setCurrentIndex(CURR_IDX);

    this->airportName = airportName;
    statYearOfDay = qMove(statistic);

    // Наполняю comboBox полученными данными,
    // перебираю все ключи,чтобы найти месяцы
    auto it = statYearOfDay.constKeyValueBegin();   // итератор на самый первый эл-т
    auto end_it = statYearOfDay.constKeyValueEnd(); // итератор на конец массива
    int oldMonth(0);
    while(it != end_it)
    {
        int month = it.base().key().month();
        if (oldMonth != month){
            oldMonth = month;
            QString date_str = MONTH_RUS.at(month - 1) + " " + QString::number(it.base().key().year());
            ui->cb_month->addItem(date_str);
            // Получается список указателей на
            // данные, проиндексированный comboBox.
            // Это данные разделенные по месяцам для графика
            listDataIt.append(it.base());
        }
        ++it;
    }
    // Добавляю итератор на конец массива
    listDataIt.append(end_it.base());
    choiseMonth(CURR_IDX);
}

// Добавляем данные на гистограмму
void Graphic::addDataToBar(QMap<QDate, int> &statistic, QString &airportName)
{
    this->airportName = airportName;
    QList<QBarSet *> listOfColumns; // список столбииов гистограммы

    for (auto it = statistic.begin(); it != statistic.end(); ++it){
        auto date = it.key();
        QString date_str = MONTH_RUS.at(date.month() - 1) + " " + QString::number(date.year());

        // выделяю память под столбики гистограммы
        listOfColumns.append( new QBarSet(date_str, this) );
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

        axisX = new QValueAxis(this);
        axisY = new QValueAxis(this);

        //chart->setTitle("Value X , Value Y");

        axisX->setRange(1, 31);
        axisX->setTickCount(10);
        axisX->setLabelFormat("%.0ui");
        //chartView->chart()->setAxisX(axisX, lineSer);

        chart->addAxis(axisX, Qt::AlignBottom);

        //lineSer->attachAxis(axisX);
        //lineSer->attachAxis(axisY);
    }

    qDebug() << "graphic show...";

    // график настроен, можно показать
    //chartView->chart()->createDefaultAxes();
    //chartView->show();
    show();
}

// клик по кнопке "Закрыть" - закроет окно графиков
void Graphic::closeGraphicWindow()
{
    airportName.clear();
    listDataIt.clear();
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

// клик по кнопке "Крестик" - закроет окно графиков
void Graphic::closeEvent(QCloseEvent *event)
{
    event->accept();
    closeGraphicWindow();
}

// слот обработки выбора из выпадающего списка месяцев
// наполняет серию в зависимости от выбора даты
void Graphic::choiseMonth(int dateIdx)
{
    // Если серия не пустая - очистить данные
    if (lineSer->count()){
        lineSer->clear();
    }

    // Итератор на начало текущего месяца
    auto currentMonth_it = listDataIt.at(dateIdx);
    // Итератор на след. месяц
    auto nextMonth_it = listDataIt.at(dateIdx + 1);

    // Наполняю серию данными текущего месяца
    while(currentMonth_it != nextMonth_it)
    {
        //qDebug() << currentMonth_it.key() << "\t" << currentMonth_it.value();
        lineSer->append(currentMonth_it.key().day(), currentMonth_it.value());
        ++currentMonth_it;
    }
}

// слот переключения вкладки tabWidget
// или click по кнопке "График" на главном окне
void Graphic::choiceTab(int tabIdx)
{
    switch (tabIdx) {
    case TabYear:
        if (barSer->count() == 0){
            emit sig_requestData(tabIdx);
        }
        else{
            chartPrepear();
        }
        break;

    case TabMonth:
        if (lineSer->count() == 0){
            emit sig_requestData(tabIdx);
        }else{
            chartPrepear();
        }
        break;

    default:
        break;
    }
}
