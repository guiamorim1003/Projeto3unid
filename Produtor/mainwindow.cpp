#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
    , timer(new QTimer(this))
    , minValue(11)
    , maxValue(19)
{
    ui->setupUi(this);

    // Inicialização dos widgets
    ui->sliderMin->setValue(minValue);
    ui->sliderMax->setValue(maxValue);
    ui->lineEditMin->setText(QString::number(minValue));
    ui->lineEditMax->setText(QString::number(maxValue));
    ui->sliderTiming->setValue(1);
    ui->labelTimingValue->setText(QString::number(1));

    setConnectedUI(false); // <-- apenas aqui!

    // Conexões dos botões e sliders
    connect(ui->pushButtonConnect, &QPushButton::clicked, this, &MainWindow::on_pushButtonConnect_clicked);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &MainWindow::on_pushButtonDisconnect_clicked);
    connect(ui->sliderMin, &QSlider::valueChanged, this, &MainWindow::on_sliderMin_valueChanged);
    connect(ui->sliderMax, &QSlider::valueChanged, this, &MainWindow::on_sliderMax_valueChanged);
    connect(ui->lineEditMin, &QLineEdit::editingFinished, this, &MainWindow::on_lineEditMin_editingFinished);
    connect(ui->lineEditMax, &QLineEdit::editingFinished, this, &MainWindow::on_lineEditMax_editingFinished);
    connect(ui->sliderTiming, &QSlider::valueChanged, this, &MainWindow::on_sliderTiming_valueChanged);
    connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::on_pushButtonStart_clicked);
    connect(ui->pushButtonStop, &QPushButton::clicked, this, &MainWindow::on_pushButtonStop_clicked);
    connect(timer, &QTimer::timeout, this, &MainWindow::sendData);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &MainWindow::onSocketError);
}

MainWindow::~MainWindow()
{
    socket->disconnectFromHost();
    delete ui;
}

void MainWindow::setConnectedUI(bool connected)
{
    ui->pushButtonConnect->setEnabled(!connected);
    ui->pushButtonDisconnect->setEnabled(connected);
    ui->pushButtonStart->setEnabled(connected);
    ui->pushButtonStop->setEnabled(false); // só habilita quando inicia o envio
    if (connected)
        ui->labelStatus->setText("connected");
    else
        ui->labelStatus->setText("disconnected");
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if (socket->state() == QAbstractSocket::ConnectedState ||
        socket->state() == QAbstractSocket::ConnectingState) {
        // Já está conectado ou tentando conectar, não faz nada
        return;
    }
    socket->connectToHost(ui->lineEditIP->text(), 1234); // Porta padrão 1234
}


void MainWindow::on_pushButtonDisconnect_clicked()
{
    socket->disconnectFromHost();
}

void MainWindow::onConnected()
{
    setConnectedUI(true);
    logMessage("Connected to server.");
}

void MainWindow::onDisconnected()
{
    setConnectedUI(false);
    logMessage("Disconnected from server.");
    timer->stop();
}

void MainWindow::onSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QMessageBox::warning(this, "Erro", socket->errorString());
    setConnectedUI(false);
}

void MainWindow::on_sliderMin_valueChanged(int value)
{
    minValue = value;
    ui->lineEditMin->setText(QString::number(value));
    if (maxValue < minValue) {
        maxValue = minValue;
        ui->sliderMax->setValue(maxValue);
    }
}

void MainWindow::on_sliderMax_valueChanged(int value)
{
    maxValue = value;
    ui->lineEditMax->setText(QString::number(value));
    if (minValue > maxValue) {
        minValue = maxValue;
        ui->sliderMin->setValue(minValue);
    }
}

void MainWindow::on_lineEditMin_editingFinished()
{
    bool ok;
    int value = ui->lineEditMin->text().toInt(&ok);
    if (ok) {
        minValue = value;
        ui->sliderMin->setValue(value);
        if (maxValue < minValue) {
            maxValue = minValue;
            ui->sliderMax->setValue(maxValue);
            ui->lineEditMax->setText(QString::number(maxValue));
        }
    }
}

void MainWindow::on_lineEditMax_editingFinished()
{
    bool ok;
    int value = ui->lineEditMax->text().toInt(&ok);
    if (ok) {
        maxValue = value;
        ui->sliderMax->setValue(value);
        if (minValue > maxValue) {
            minValue = maxValue;
            ui->sliderMin->setValue(minValue);
            ui->lineEditMin->setText(QString::number(minValue));
        }
    }
}

void MainWindow::on_sliderTiming_valueChanged(int value)
{
    ui->labelTimingValue->setText(QString::number(value));
    if (timer->isActive())
        timer->setInterval(value * 1000);
}

void MainWindow::on_pushButtonStart_clicked()
{
    int interval = ui->sliderTiming->value() * 1000;
    timer->start(interval);
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
    logMessage("Started sending data every " + QString::number(ui->sliderTiming->value()) + "s.");
}

void MainWindow::on_pushButtonStop_clicked()
{
    timer->stop();
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    logMessage("Stopped sending data.");
}

void MainWindow::sendData()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        qint64 msecdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
        int valor = QRandomGenerator::global()->bounded(minValue, maxValue+1);
        QString msg = QString("set %1 %2\r\n").arg(QString::number(msecdate)).arg(QString::number(valor));
        socket->write(msg.toUtf8());
        socket->flush();
        logMessage(msg.trimmed());
    }
}

void MainWindow::logMessage(const QString &msg)
{
    ui->listWidgetDados->addItem(msg);
    ui->listWidgetDados->scrollToBottom();
}
