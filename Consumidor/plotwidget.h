#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QVector>

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

    // Chame isso para atualizar os dados do gráfico
    void setData(const QVector<double>& data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<double> m_data;
};

#endif // PLOTWIDGET_H
