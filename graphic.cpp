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
    chartBar = new QChart();
    chartLine = new QChart();
    chartBar->legend()->hide();
    chartLine->legend()->hide();

    // Объект QChartView является виджетом отображальщиком графика. В его
    // конструктор необходимо передать ссылку на объект QChart.
    viewBar = new QChartView(chartBar);
    viewLine = new QChartView(chartLine);

    // Размещает виджеты по ячейкам таблицы?!
    ui->layoutBar->addWidget(viewBar);
    ui->layoutLine->addWidget(viewLine);


    connect(ui->pushButton, &QPushButton::clicked, this, &Graphic::closeGraphicWindow);
    connect(ui->cb_month, SIGNAL(activated(int)), this, SLOT(choiseMonth(int)));
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &Graphic::choiceTab);
}

Graphic::~Graphic()
{
    setup.saveGeometryWidget(this); // сохраняю геометрию виджета

    delete chartBar;
    delete chartLine;

    delete viewBar;
    delete viewLine;

    delete ui;
}

// Добавляем данные
void Graphic::addData(Tab idx, QMap<QDate, int> &statistic, QString &airportName)
{
    ui->lb_airportName->setText("Аэропорт \"" + airportName + "\"");

    switch (idx) {
    case TabYear:
    {
        // Добавляем данные на гистограмму
        // выделяю память под набор столбиков для категории "Bar"
        myBarSet = new QBarSet("Bar", this);
        int max(0);

        for (auto it = statistic.begin(); it != statistic.end(); ++it){
            auto date = it.key();
            QString date_str = MONTH_RUS.at(date.month() - 1) + " " + QString::number(date.year());

            categories.append(date_str);    // список категорий
            myBarSet->append(it.value());   // вношу данные в столбики

            if (max < it.value()){
                max = it.value();
            }
        }

        // Присоединаю серию к графику
        chartBar->addSeries(barSer);
        // Наполняю серию
        barSer->append(myBarSet);

        // Завожу ось X
        axisBarX = new QBarCategoryAxis(this);
        axisBarX->append(categories);
        axisBarX->setLabelsAngle(270);
        chartBar->addAxis(axisBarX, Qt::AlignBottom);
        barSer->attachAxis(axisBarX);

        // Завожу ось Y
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
        // Добавляем данные на график
        ui->cb_month->clear();
        const int CURR_IDX(0);
        ui->cb_month->setCurrentIndex(CURR_IDX);

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
        break;
    }

    default:
        break;
    }
}

// клик по кнопке "Закрыть" - вызовет "closeEvent"
void Graphic::closeGraphicWindow()
{
    close();
}

// клик по кнопке "Крестик" - закроет окно графиков
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
    // если серия не пустая - очистить данные
    if (barSer->count()){
        barSer->clear();
    }
    // если серия не пустая - очистить данные
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

// слот обработки выбора из выпадающего списка месяцев
// наполняет серию в зависимости от выбора даты
void Graphic::choiseMonth(int dateIdx)
{
    // Если серия не пустая - очистить данные
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

    // Итератор на начало текущего месяца
    auto currentMonth_it = listDataIt.at(dateIdx);
    //int firstDay = currentMonth_it.key().day();
    // Итератор на след. месяц
    auto nextMonth_it = listDataIt.at(dateIdx + 1);

    // Наполняю серию данными текущего месяца
    while(currentMonth_it != nextMonth_it)
    {
        //qDebug() << currentMonth_it.key() << "\t" << currentMonth_it.value();
        lineSer->append(currentMonth_it.key().day(), currentMonth_it.value());
        ++currentMonth_it;
    }

    // Присоединаю серию к графику
    chartLine->addSeries(lineSer);

    // Настройка осей графика
    //int lastDay = (--nextMonth_it).key().day();
    // Завожу ось X
    axisLineX = new QValueAxis(this);
    //axisLineX->setRange(firstDay, lastDay);
    axisLineX->setTitleText("Числа месяца");
    axisLineX->setLabelFormat("%u");
    axisLineX->setTickCount(1);
    chartLine->addAxis(axisLineX, Qt::AlignBottom);
    lineSer->attachAxis(axisLineX);

    // Завожу ось Y
    axisLineY = new QValueAxis(this);
    axisLineY->setTitleText("Кол-во прилетов/вылетов");
    axisLineY->setLabelFormat("%u");
    axisLineY->setTickCount(5);
    chartLine->addAxis(axisLineY, Qt::AlignLeft);
    lineSer->attachAxis(axisLineY);
}

// слот переключения вкладки tabWidget
// или click по кнопке "График" на главном окне
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
