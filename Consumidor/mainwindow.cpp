#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
    , timer(new QTimer(this))
{
    ui->setupUi(this);

    // Liga botões aos slots
    connect(ui->pushButtonConnect, &QPushButton::clicked, this, &MainWindow::on_pushButtonConnect_clicked);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &MainWindow::on_pushButtonDisconnect_clicked);
    connect(ui->pushButtonUpdate, &QPushButton::clicked, this, &MainWindow::on_pushButtonUpdate_clicked);
    connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::on_pushButtonStart_clicked);
    connect(ui->pushButtonStop, &QPushButton::clicked, this, &MainWindow::on_pushButtonStop_clicked);
    connect(ui->sliderTiming, &QSlider::valueChanged, this, &MainWindow::onSliderTimingChanged);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(timer, &QTimer::timeout, this, &MainWindow::getDataFromServer);

    // Configurações iniciais
    ui->sliderTiming->setMinimum(1);
    ui->sliderTiming->setMaximum(10);
    ui->sliderTiming->setValue(1);
    ui->labelTimingValue->setText(QString::number(ui->sliderTiming->value()));

    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
}

MainWindow::~MainWindow()
{
    socket->close();
    delete ui;
}

// --- Slots dos botões ---

void MainWindow::on_pushButtonConnect_clicked()
{
    QString ip = ui->lineEditIP->text();
    socket->connectToHost(ip, 1234); // Altere a porta se necessário

    if (!socket->waitForConnected(3000)) {
        QMessageBox::warning(this, "Erro", "Não foi possível conectar ao servidor.");
    } else {
        QMessageBox::information(this, "Conectado", "Conectado ao servidor.");
        ui->pushButtonConnect->setEnabled(false);
        ui->pushButtonDisconnect->setEnabled(true);
    }
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    socket->disconnectFromHost();
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
}

void MainWindow::on_pushButtonUpdate_clicked()
{
    // Gera dados aleatórios para teste do gráfico
    QVector<double> dados;
    for(int i=0; i<20; ++i)
        dados.append(QRandomGenerator::global()->bounded(100));
    updatePlot(dados);
}

void MainWindow::on_pushButtonStart_clicked()
{
    int intervalo = ui->sliderTiming->value() * 1000;
    timer->start(intervalo);
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
}

void MainWindow::on_pushButtonStop_clicked()
{
    timer->stop();
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
}

void MainWindow::onSliderTimingChanged(int value)
{
    ui->labelTimingValue->setText(QString::number(value));
    if (timer->isActive()) {
        timer->setInterval(value * 1000);
    }
}

// --- Comunicação TCP (adaptado para seu projeto) ---

void MainWindow::getDataFromServer()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write("get 127.0.0.1 20\r\n"); // Ajuste conforme comando do seu servidor
        socket->flush();
    }
}

void MainWindow::onSocketReadyRead()
{
    // Espera receber várias linhas do servidor: cada linha = "timestamp valor"
    QVector<double> dados;
    while (socket->canReadLine()) {
        QString linha = QString::fromUtf8(socket->readLine()).trimmed();
        QStringList partes = linha.split(" ");
        if (partes.size() == 2) {
            bool ok;
            double valor = partes[1].toDouble(&ok);
            if (ok) dados.append(valor);
        }
    }
    if (!dados.isEmpty()) {
        updatePlot(dados);
    }
}

// --- Atualização do gráfico ---

void MainWindow::updatePlot(const QVector<double>& data)
{
    ui->widgetPlot->setData(data);
}

