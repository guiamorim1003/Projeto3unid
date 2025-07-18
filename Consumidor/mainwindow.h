#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonUpdate_clicked();
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void onSocketReadyRead();
    void onSliderTimingChanged(int value);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTimer *timer;
    QVector<double> plotData;
    void updatePlot(const QVector<double>& data);
    void getDataFromServer();
};

#endif // MAINWINDOW_H
