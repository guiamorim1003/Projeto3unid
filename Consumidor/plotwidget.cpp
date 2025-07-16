#include "plotwidget.h"
#include <QPainter>
#include <QPen>
#include <algorithm>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(200, 200); // Só para garantir um tamanho mínimo legal
}

void PlotWidget::setData(const QVector<double> &data)
{
    m_data = data;
    update(); // Redesenha o gráfico
}

void PlotWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    double margin = 10;
    QRectF area(margin, margin, width()-2*margin, height()-2*margin);

    // Fundo amarelo (apenas na área interna)
    painter.setBrush(QBrush(Qt::yellow));
    painter.setPen(Qt::NoPen);
    painter.drawRect(area);

    if (m_data.size() < 2)
        return;

    double minY = *std::min_element(m_data.begin(), m_data.end());
    double maxY = *std::max_element(m_data.begin(), m_data.end());
    if (minY == maxY) maxY += 1;

    double w = area.width();
    double h = area.height();
    double xStep = w / (m_data.size() - 1);

    // Linha azul
    QPen pen(Qt::blue, 2);
    painter.setPen(pen);

    for (int i = 0; i < m_data.size() - 1; ++i) {
        double x1 = area.left() + i * xStep;
        double y1 = area.top() + h - ((m_data[i] - minY) / (maxY - minY)) * h;
        double x2 = area.left() + (i + 1) * xStep;
        double y2 = area.top() + h - ((m_data[i+1] - minY) / (maxY - minY)) * h;
        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }

    // Contorno vermelho (apenas na área interna)
    painter.setPen(QPen(Qt::red, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(area);
}
