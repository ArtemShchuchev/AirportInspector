#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QWidget>

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

private:
    Ui::Graphic *ui;
};

#endif // GRAPHIC_H
