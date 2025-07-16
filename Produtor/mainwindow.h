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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_sliderMin_valueChanged(int value);
    void on_sliderMax_valueChanged(int value);
    void on_lineEditMin_editingFinished();
    void on_lineEditMax_editingFinished();
    void on_sliderTiming_valueChanged(int value);
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void sendData();
    void onConnected();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTimer *timer;

    int minValue;
    int maxValue;
    void setConnectedUI(bool connected);
    void logMessage(const QString &msg);
};

#endif // MAINWINDOW_H
