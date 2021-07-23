#include "tcpservermonitor.h"

TcpServerMonitor::TcpServerMonitor(QWidget *parent) : QWidget(parent)
{
    m_tcpServer = nullptr;

    // Server status
    QGroupBox *groupBoxSocketStatus = new QGroupBox("Server Status");

    QGridLayout *layoutSocketStatus = new QGridLayout;

    m_pushButtonStart = new QPushButton("Start listening");
    m_pushButtonStop = new QPushButton("Stop listening");
    connect(m_pushButtonStart, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStart_clicked);
    connect(m_pushButtonStop, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStop_clicked);

    m_labelSocketStatus = new QLabel;
    m_labelServerIP = new QLabel;
    m_labelServerPort = new QLabel;
    m_labelClientIP = new QLabel;
    m_labelClientPort = new QLabel;

    layoutSocketStatus->addWidget(m_labelSocketStatus, 0, 0);
    layoutSocketStatus->addWidget(m_labelClientIP, 0, 2);
    layoutSocketStatus->addWidget(m_labelClientPort, 0, 3);
    layoutSocketStatus->addWidget(m_pushButtonStart, 1, 0);
    layoutSocketStatus->addWidget(m_pushButtonStop, 1, 1);
    layoutSocketStatus->addWidget(m_labelServerIP, 1, 2);
    layoutSocketStatus->addWidget(m_labelServerPort, 1, 3);

    groupBoxSocketStatus->setLayout(layoutSocketStatus);

    // Receive data
    QGroupBox *groupBoxReceiveData = new QGroupBox("Receive data");

    QTextBrowser *textBrowserReceiveData = new QTextBrowser;

    groupBoxReceiveData->setLayout(new QVBoxLayout);
    groupBoxReceiveData->layout()->addWidget(textBrowserReceiveData);

    // Send data
    QGroupBox *groupBoxSendData = new QGroupBox("Send data");

    QTextEdit *textEditSendData = new QTextEdit;

    groupBoxSendData->setLayout(new QVBoxLayout);
    groupBoxSendData->layout()->addWidget(textEditSendData);

    // Send data options
    QGroupBox *groupBoxSendOptions = new QGroupBox("Send options");

    m_comboBoxTcpSocket = new QComboBox;
    QComboBox *comboBoxNumberSend = new QComboBox;
    QLineEdit *lineEditInterval = new QLineEdit;
    QPushButton *pushButtonSend = new QPushButton("Send data");
    QPushButton *pushButtonSendPeriod = new QPushButton("Cycle sending");
    QPushButton *pushButtonStopSend = new QPushButton("Stop sending");
    connect(pushButtonSend, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonSend_clicked);
    connect(pushButtonSendPeriod, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonSendPeriod_clicked);
    connect(pushButtonStopSend, &QPushButton::clicked, this,
            &TcpServerMonitor::on_pushButtonStopSend_clicked);

    QGridLayout *gridLayoutSendOptions = new QGridLayout;

    gridLayoutSendOptions->addWidget(new QLabel("Destination"), 0, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(new QLabel("Send times"), 1, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(new QLabel("Send interval"), 2, 0,
                                     Qt::AlignRight);
    gridLayoutSendOptions->addWidget(m_comboBoxTcpSocket, 0, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(comboBoxNumberSend, 1, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(lineEditInterval, 2, 1, Qt::AlignLeft);
    gridLayoutSendOptions->addWidget(pushButtonSend, 0, 2);
    gridLayoutSendOptions->addWidget(pushButtonSendPeriod, 1, 2);
    gridLayoutSendOptions->addWidget(pushButtonStopSend, 2, 2);

    groupBoxSendOptions->setLayout(gridLayoutSendOptions);

    // Layout
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    vBoxLayout->addWidget(groupBoxSocketStatus);
    vBoxLayout->addWidget(groupBoxReceiveData);
    vBoxLayout->addWidget(groupBoxSendData);
    vBoxLayout->addWidget(groupBoxSendOptions);

    setLayout(vBoxLayout);
}

TcpServerMonitor::~TcpServerMonitor()
{
    if (m_tcpServer != nullptr)
    {
        m_tcpServer->close();
        m_tcpServer->deleteLater();
    }
}

void TcpServerMonitor::setTcpServer(QTcpServer *tcpServer)
{
    Q_ASSERT(m_tcpServer == nullptr);
    if (tcpServer == nullptr)
    {
        return;
    }

    m_tcpServer = tcpServer;
    m_hostAddress = m_tcpServer->serverAddress();
    m_port = m_tcpServer->serverPort();

    connect(m_tcpServer, &QTcpServer::newConnection, this,
            &TcpServerMonitor::on_tcpServer_newConnection);
    // connect(m_tcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)),
    // this, SLOT(acceptSocketError(QAbstractSocket::SocketError)));
    m_labelServerIP->setText(
        QString("Server IP: %1").arg(m_hostAddress.toString()));
    m_labelServerPort->setText(
        QString("Server port: %1").arg(QString::number(m_port)));

    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelSocketStatus->setText("Listening");
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelSocketStatus->setText("Stop");
    }
}

void TcpServerMonitor::on_pushButtonStart_clicked()
{
    if (!m_tcpServer->listen(m_hostAddress, m_port))
    {
        QString estr = QString("Start TCP server failed! %1")
                           .arg(m_tcpServer->errorString());
        QMessageBox::critical(this, tr("Error"), estr);
    }
    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelSocketStatus->setText("Listening");
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelSocketStatus->setText("Stop");
    }
}

void TcpServerMonitor::on_pushButtonStop_clicked()
{
    m_tcpServer->close();
    if (m_tcpServer->isListening())
    {
        m_pushButtonStart->setEnabled(false);
        m_pushButtonStop->setEnabled(true);
        m_labelSocketStatus->setText("Listening");
    }
    else
    {
        m_pushButtonStart->setEnabled(true);
        m_pushButtonStop->setEnabled(false);
        m_labelSocketStatus->setText("Stop");
    }
}

void TcpServerMonitor::on_pushButtonSend_clicked() {}

void TcpServerMonitor::on_pushButtonSendPeriod_clicked() {}

void TcpServerMonitor::on_pushButtonStopSend_clicked() {}

void TcpServerMonitor::on_tcpServer_newConnection()
{
    QTcpSocket *tcpSocket = m_tcpServer->nextPendingConnection();
    m_tcpSockets.append(tcpSocket);
    QHostAddress remoteIp = tcpSocket->peerAddress();
    quint16 remotePort = tcpSocket->peerPort();
    m_comboBoxTcpSocket->addItem(QString("%1:%2").arg(remoteIp.toString().arg(
                                     QString::number(remotePort))),
                                 QVariant::fromValue(tcpSocket));
}
